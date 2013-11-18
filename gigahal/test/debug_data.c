

#include "test.h"
#include "symbol_set.h"
#include "chain.h"

static void header(const char * text)
{
    printf("---------------- %s ---------------\n", text);
}

static void print_chain(GhBrain * gh, bool back)
{
    for (size_t i = 0; i <= gh->chain_mask; i++) {
        GhChain * chain = gh->chains[i];
        while (chain != NULL) {

            printf("'%s', '%s', '%s', '%s' (%d)\n",
                chain->q.a->data,
                chain->q.b->data,
                chain->q.c->data,
                chain->q.d->data,
                chain->usage);

            GhSetIterator it = gh_sset_iterator(back ? &chain->back : &chain->forw);
            GhSymbol * s;
            size_t usage;
            while ((s = gh_sset_nextu(&it, &usage)) != NULL)
                printf("  '%s' (%zu)\n", s->data, usage);

            printf("\n");

            chain = chain->next;
        }
    }
}

static void print_symbols(GhBrain * gh, bool pointers)
{
    for (size_t i = 0; i <= gh->symbol_mask; i++) {

        GhSymbol * symbol = gh->symbols[i];

        while (symbol != NULL) {

            printf("%s (%d)\n", symbol->data, symbol->usage);

            if (gh_symbol_isword(symbol) && pointers) {
                for (size_t j = 0; j <= symbol->chp_mask; j++) {
                    GhChain * chain = symbol->chp[j];
                    if (chain != NULL) {
                        printf("  ('%s', '%s', '%s', '%s')\n",
                            chain->q.a->data,
                            chain->q.b->data,
                            chain->q.c->data,
                            chain->q.d->data);
                    }
                }
            }
            symbol = symbol->next;
        }
    }
}

int main(int argc, const char * argv[])
{
    if (argc <= 1) {
        printf("Usage: debug_data [fbsp]\n");
        printf("  f - print chains with forward symbols\n");
        printf("  b - print chains with backward symbols\n");
        printf("  s - print list of symbols \n");
        printf("  p - print list of symbols with chain pointers (implies s)\n");
        exit(0);
    }

    printf("\n");

    const char * options = argv[1];

    GhBrain * gh = gh_new_brain();

    gh_input_no_reply(gh, "This is test sentence number one.");
    gh_input_no_reply(gh, "Different sentence to provide additional input");

    if (strchr(options, 'f')) {
        header("Forward chain");
        print_chain(gh, false);
        printf("\n");
    }

    if (strchr(options, 'b')) {
        header("Backward chain");
        print_chain(gh, true);
        printf("\n");
    }

    if (strchr(options, 's') || strchr(options, 'p')) {
        header("Symbols");
        print_symbols(gh, strchr(options, 'p'));
        printf("\n");
    }
}
