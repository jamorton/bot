
#include "test.h"
#include "symbol.h"
#include "symbol_vec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GhSymbol * symbs[99999999];

int main(void)
{
    GhBrain * gh = gh_new_brain();
    WordList list = load_words("test/words.txt");
    GhSvec * vec = gh_svec_s(32);

    for (size_t i = 0; i < list.len; i++) {

        const char * str = list.words[i];
        GhSymbol * s1 = gh_symbol_new(gh, str, 0);
        GhSymbol * s2 = gh_symbol_new(gh, str, 0);
        if (!gh_symbol_eq(s1, s2))
            printf("Not the same!\n");
        if (strcmp(gh_symbol_data(s1), gh_symbol_data(s2)) != 0)
            printf("Strcmp fail!\n");
        if (i < 30)
            gh_svec_push(vec, s1);
        symbs[i++] = s1;
	}

    GH_SVEC_FOREACH(vec, s) {
        printf("%s\n", gh_symbol_data(s));
    }

    gh_svec_del(vec);
    return 0;
}
