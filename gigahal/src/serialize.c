
#include "gigahal.h"
#include "symbol.h"
#include "chain.h"
#include "symbol_set.h"
#include "symbol_vec.h"

#include <unistd.h>
#include <string.h>
#include <errno.h>

#define FORMAT_VERSION 1

GH_AINLINE void write_uint(FILE * f, uint64_t num)
{
    fwrite(&num, sizeof(uint64_t), 1, f);
}

GH_AINLINE uint64_t read_uint(FILE * f)
{
    uint64_t ret = 0;
    gh_assert(fread(&ret, sizeof(ret), 1, f) == 1);
    return ret;
}

GH_AINLINE void write_symbol(FILE * f, GhSymbol * symbol)
{
    if (symbol == GH_SYMBOL_NONE)
        write_uint(f, 0);
    else
        write_uint(f, symbol->hash);
}


static void serialize(GhBrain * gh, FILE * f)
{
    fputc('g', f);
    fputc('h', f);
    write_uint(f, FORMAT_VERSION);

    write_uint(f, gh->max_symbol_usage);
    write_uint(f, gh->symbol_count);

    hash_t count = 1;
    size_t i;

    for (i = 0; i <= gh->symbol_mask; i++) {
        GhSymbol * symbol = gh->symbols[i];
        while (symbol != NULL) {
            symbol->hash = count++;

            write_uint(f, symbol->usage);
            write_uint(f, symbol->len);
            write_uint(f, symbol->flags);

            fwrite(symbol->data, symbol->len, 1, f);
            symbol = symbol->next;
        }
    }

    count = 0;
    write_uint(f, gh->chain_count);

    for (i = 0; i <= gh->chain_mask; i++) {
        GhChain * chain = gh->chains[i];
        while (chain != NULL) {
            chain->hash = count++;
            write_symbol(f, chain->q.a);
            write_symbol(f, chain->q.b);
            write_symbol(f, chain->q.c);
            write_symbol(f, chain->q.d);
            write_uint(f, chain->usage);
            write_uint(f, chain->forw.len);
            write_uint(f, chain->back.len);

            size_t j;
            GhSetBucket * table = chain->forw.table;
            for (j = 0; j <= chain->forw.mask; j++) {
                if (table[j].symbol) {
                    write_symbol(f, table[j].symbol);
                    write_uint(f, table[j].usage);
                }
            }
            table = chain->back.table;
            for (j = 0; j <= chain->back.mask; j++) {
                if (table[j].symbol) {
                    write_symbol(f, table[j].symbol);
                    write_uint(f, table[j].usage);
                }
            }
            chain = chain->next;
        }
    }

    for (i = 0; i <= gh->symbol_mask; i++) {
        GhSymbol * symbol = gh->symbols[i];
        while (symbol != NULL) {
            write_uint(f, symbol->chp_len);

            if (symbol->chp_len > 0)
                for (size_t j = 0; j <= symbol->chp_mask; j++)
                    if (symbol->chp[j])
                        write_uint(f, symbol->chp[j]->hash);

            symbol = symbol->next;
        }
    }

    /* we used the symbol/chain `hash` members to store their serialization
       id. need to recompute them now */
    for (i = 0; i <= gh->symbol_mask; i++) {
        GhSymbol * symbol = gh->symbols[i];
        while (symbol != NULL) {
            symbol->hash = gh_hash_string(symbol->data, symbol->len);
            symbol = symbol->next;
        }
    }

    for (i = 0; i < gh->chain_mask; i++) {
        GhChain * chain = gh->chains[i];
        while (chain != NULL) {
            chain->hash = gh_hash_quad(&chain->q);
            chain = chain->next;
        }
    }
}

static GhBrain * unserialize(FILE * f)
{
    if (fgetc(f) != 'g' || fgetc(f) != 'h') {
        fprintf(stderr, "gh_unseralize: Invalid file format");
        return NULL;
    }

    uint64_t fversion = read_uint(f);

    GhBrain * gh = gh_new_brain_default();
    gh->max_symbol_usage = (size_t)read_uint(f);

    size_t i, num_symbols = (size_t)read_uint(f);

    GhSymbol * symbols[num_symbols + 1];
    symbols[0] = GH_SYMBOL_NONE;

    for (i = 1; i <= num_symbols; i++) {
        uint32_t usage = (uint32_t)read_uint(f);
        uint16_t len   = (uint16_t)read_uint(f);
        uint8_t  flags =  (uint8_t)read_uint(f);

        char buf[len];
        fread(buf, len, 1, f);

        symbols[i] = gh_symbol_newl(gh, buf, len, flags);
        symbols[i]->usage = usage;
    }

    size_t num_chains = (size_t)read_uint(f);
    GhChain * chains[num_chains];

    for (i = 0; i < num_chains; i++) {
        GhSymbol * a = symbols[read_uint(f)];
        GhSymbol * b = symbols[read_uint(f)];
        GhSymbol * c = symbols[read_uint(f)];
        GhSymbol * d = symbols[read_uint(f)];
        GhQuad q = {a, b, c, d};

        GhChain * chain = gh_chain_add(gh, &q);
        chains[i] = chain;

        chain->usage = (uint32_t)read_uint(f);
        size_t num_forw = (size_t)read_uint(f);
        size_t num_back = (size_t)read_uint(f);

        size_t j;
        for (j = 0; j < num_forw; j++) {
            GhSymbol * add = symbols[read_uint(f)];
            gh_sset_add(&chain->forw, add);
            gh_sset_set_usage(&chain->forw, add, (size_t)read_uint(f));
        }
        for (j = 0; j < num_back; j++) {
            GhSymbol * add = symbols[read_uint(f)];
            gh_sset_add(&chain->back, add);
            gh_sset_set_usage(&chain->back, add, (size_t)read_uint(f));
        }
    }

    for (i = 1; i <= num_symbols; i++) {
        size_t chp_count = (size_t)read_uint(f);
        GhSymbol * symbol = symbols[i];
        for (size_t j = 0; j < chp_count; j++)
            gh_symbol_add_chain(symbol, chains[read_uint(f)]);
    }

    return gh;
}

void gh_save(GhBrain * gh, const char * path)
{
    char tmp[256];
    sprintf(tmp, "%s.%d.tmp", path, (int)getpid());
    FILE * f = fopen(tmp, "wb");
    if (f == NULL) {
        fprintf(stderr, "Failed opening temp file for saving: %s\n", strerror(errno));
        return;
    }
    serialize(gh, f);
    fclose(f);
    if (rename(tmp, path) != 0)
        fprintf(stderr, "Failed to move data file into place: %s\n", strerror(errno));
}

GhBrain * gh_load(const char * filename)
{
    FILE * f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "Failed loading file %s: %s\n", filename, strerror(errno));
        return NULL;
    }
    GhBrain * ret = unserialize(f);
    fclose(f);
    return ret;
}
