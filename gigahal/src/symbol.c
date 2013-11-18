
#include "symbol.h"
#include "chain.h"

#include <stdio.h>
#include <string.h>

GhSymbol gh_null_symbol = {NULL, 0, 0, NULL, 0, 0, 0, 0, ""};

GH_AINLINE void resize_table(GhBrain * gh)
{
    size_t new_mask = (gh->symbol_mask << 1) + 1;
    GhSymbol ** new_symbols = gh_new_array(GhSymbol *, new_mask + 1);

    for (size_t i = gh->symbol_mask; i != ~(size_t)0; --i) {
        GhSymbol * symbol = gh->symbols[i];
        while (symbol != NULL) {
            size_t bucket = symbol->hash & new_mask;
            GhSymbol * next = symbol->next;
            symbol->next = new_symbols[bucket];
            new_symbols[bucket] = symbol;
            symbol = next;
        }
    }

    gh_free(gh->symbols);
    gh->symbols = new_symbols;
    gh->symbol_mask = new_mask;
}

GH_AINLINE void check_resize(GhBrain * gh)
{
    /* allow 100% load factor before resizing */
    if (gh_unlikely(gh->symbol_count > gh->symbol_mask))
        resize_table(gh);
}

GH_AINLINE GhSymbol * alloc_symbol(const char * str, uint16_t len, uint8_t flags)
{
    GhSymbol * ret = (GhSymbol *)gh_falloc(sizeof(GhSymbol) + len);
    ret->next = NULL;
    ret->flags = flags;
    ret->len = len;
    ret->usage = 0;
    memcpy(ret->data, str, len);
    ret->data[len] = '\0';
    ret->chp_mask = ret->chp_len = 0;

    if (flags & GH_SYMBOL_ISWORD) {
        ret->chp_len = 0;
        ret->chp_mask = 7;
        ret->chp = gh_new_array(GhChain *, ret->chp_mask + 1);
    }

    return ret;
}

void gh_symbol_init(GhBrain * gh)
{
    gh->symbol_mask = 0xFFF; // 4096 buckets
    gh->symbols = gh_new_array(GhSymbol *, gh->symbol_mask + 1);
    gh->symbol_count = 0;
}

void gh_symbol_deinit(GhBrain * gh)
{
    for (size_t i = 0; i <= gh->symbol_mask; i++) {
        GhSymbol * symbol = gh->symbols[i];
        while (symbol != NULL) {
            GhSymbol * next = symbol->next;
            if (gh_symbol_isword(symbol))
                gh_free(symbol->chp);
            gh_free(symbol);
            symbol = next;
        }
    }
    gh_free(gh->symbols);
}

GhSymbol * gh_symbol_newl(GhBrain * gh, const char * str, size_t len_, uint8_t flags)
{
    if (len_ > UINT16_MAX || len_ == 0)
        return NULL;

    uint16_t len = (uint16_t)len_;
    hash_t hash = gh_hash_string(str, len);
    size_t bucket = hash & gh->symbol_mask;

    GhSymbol * symbol = gh->symbols[bucket];
    size_t cur = 0;

    /* scan hash bucket for exiting symbol */
    while (symbol != NULL) {
        const char * bytes = gh_symbol_data(symbol);

        if (symbol->len == len &&
            memcmp(bytes, str, len) == 0) {
            symbol->flags |= flags;
            return symbol;
       }
        symbol = symbol->next;
    }

    /* non found: perform insert */
    GhSymbol * ins = alloc_symbol(str, len, flags);
    ins->next = gh->symbols[bucket];
    ins->hash = hash;
    gh->symbols[bucket] = ins;
    gh->symbol_count++;

    check_resize(gh);

    return ins;
}

GhSymbol * gh_symbol_new(GhBrain * gh, const char * str, uint8_t flags)
{
    return gh_symbol_newl(gh, str, strlen(str), flags);
}

GH_AINLINE void chp_resize(GhSymbol * symbol)
{
    size_t nmask = (symbol->chp_mask << 1) + 1;
    GhChain ** ntable = gh_new_array(GhChain *, nmask);
    for (size_t i = symbol->chp_mask; i != ~(size_t)0; --i) {
        GhChain * chain = symbol->chp[i];
        if (chain == NULL)
            continue;
        for (size_t j = 0; j <= nmask; j++) {
            size_t idx = (chain->hash + j) & nmask;
            if (ntable[idx] != NULL)
                continue;
            ntable[idx] = chain;
            break;
        }
    }
    gh_free(symbol->chp);
    symbol->chp_mask = nmask;
    symbol->chp = ntable;
}

void gh_symbol_add_chain(GhSymbol * symbol, GhChain * chain)
{
    gh_assert(gh_symbol_isword(symbol));

    for (size_t i = 1; i <= symbol->chp_mask; i++) {
        size_t idx = (chain->hash + i/2 + i*i/2) & symbol->chp_mask;
        if (symbol->chp[idx] == chain)
            return;
        if (symbol->chp[idx] == NULL) {
            symbol->chp[idx] = chain;
            symbol->chp_len++;
            if (gh_unlikely(symbol->chp_len * 4 > symbol->chp_mask * 3))
                chp_resize(symbol);
            break;
        }
    }
}
