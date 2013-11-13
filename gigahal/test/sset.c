
#include "test.h"
#include "symbol.h"
#include "symbol_set.h"

/* testing symbol_setss */

int main(void)
{

    GhBrain * gh = gh_new_brain(gh_config_default);

    GhSymbolSet * set = gh_sset_new();

    GhSymbol * s1 = gh_symbol_new(gh, "hello", 0);
    GhSymbol * s2 = gh_symbol_new(gh, "world", 0);

    if (gh_sset_has(set, s1))
        printf("Fail.\n");

    gh_sset_add(set, s1);

    if (!gh_sset_has(set, s1) || gh_sset_has(set, s2))
        printf("Fail.\n");

    gh_sset_add(set, s2);

    if (!gh_sset_has(set, s2))
        printf("Fail.\n");

    GhSetIterator it = gh_sset_iterator(set);
    GhSymbol * sym;
    while ((sym = gh_sset_next(&it)) != NULL) {
        printf("  %s\n", gh_symbol_data(sym));
    }

    WordList list = load_words("test/words.txt");

    clock_t start = clock();

    for (size_t i = 0; i < list.len; i++) {

        GhSymbol * s = gh_symbol_new(gh, list.words[i], 0);
        gh_sset_add(set, s);

        if (!gh_sset_has(set, s))
            printf("Fail.\n");
    }

    float time = (float)(clock() - start) / CLOCKS_PER_SEC;
    float wps = list.len / time;
    printf("symbol_set: %zu words in %.2f sec (%.2f words/sec)\n", list.len, time, wps);
}
