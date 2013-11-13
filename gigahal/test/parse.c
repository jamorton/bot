
#include "gigahal.h"
#include "symbol.h"
#include "symbol_vec.h"
#include "syntax.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char * strings[] = {
    "One two three",
    "\t Hello...    my name is \"Test\". ",
    "odd.sentence",
    "i n `5gt7j89ko#_}Q({Fo3-.dx/;clrx5k89%Qfgf",
    "Jill's test for 'apostrophes' + $100 #miley thing-doing -- test",
    "tyler@OFWGKTA.com 100% :) yes+no",
};

int main(void)
{
    GhBrain * gh = gh_new_brain(gh_config_default);

    for (unsigned int i = 0; i < sizeof(strings)/sizeof(char *); i++) {
        GhSvec * symbols = gh_svec();
        gh_parse_symbols(gh, symbols, strings[i]);
        printf("\"%s\"\n", strings[i]);
        printf("-> ");
        GH_SVEC_FOREACH(symbols, s)
            printf("'%s', ", gh_symbol_data(s));
        printf("\n\n");

        gh_svec_del(symbols);
    }
}
