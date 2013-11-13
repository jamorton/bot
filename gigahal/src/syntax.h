
/**
 * Methods for parsing/producing entire messages (at the sentence level)
 * String -> list of symbols, list of symbols -> string, etc.
 */

#ifndef GH_SYNTAX_H
#define GH_SYNTAX_H

#include "gigahal.h"
#include "symbol_vec.h"

void gh_parse_symbols(GhBrain * gh, GhSvec * vec, const char * str);

char * gh_join_symbols(GhBrain * gh, GhSymbol ** symbols, size_t len);

#endif
