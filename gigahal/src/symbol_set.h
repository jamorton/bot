
/**
 * GhSymbolSet
 *
 * A unique, unsorted set of GhSymbol
 * Additionally keeps track of number of times each symbol has been added as
 * a duplicate
 */

#ifndef GH_SYMBOL_SET_H
#define GH_SYMBOL_SET_H

#include "symbol.h"

typedef struct GhSetBucket {
    GhSymbol * symbol;
    size_t usage;
} GhSetBucket;

typedef struct GhSymbolSet {
    uint32_t len;
    uint32_t mask;
    GhSetBucket * table;
} GhSymbolSet;

typedef struct {
    GhSymbolSet * set;
    GhSetBucket * cur;
    size_t i;
} GhSetIterator;

GhSymbolSet * gh_sset_new(void);
void gh_sset_init(GhSymbolSet * set, uint32_t initial_mask);

bool gh_sset_has(GhSymbolSet * set, GhSymbol * symbol);
bool gh_sset_add(GhSymbolSet * set, GhSymbol * symbol);

void gh_sset_set_usage(GhSymbolSet * set, GhSymbol * symbol, size_t usage);

void gh_sset_del(GhSymbolSet * set);

GH_AINLINE GhSetIterator gh_sset_iterator(GhSymbolSet * set)
{
    GhSetIterator ret = {.set = set, .cur = NULL, .i = 0};
    return ret;
}

GH_AINLINE GhSymbol * gh_sset_next(GhSetIterator * it)
{
    while (it->i <= it->set->mask) {
        GhSymbol * s = it->set->table[it->i++].symbol;
        if (s != NULL)
            return s;
    }
    return NULL;
}

GH_AINLINE GhSymbol * gh_sset_nextu(GhSetIterator * it, size_t * usage)
{
    while (it->i <= it->set->mask) {
        GhSetBucket b = it->set->table[it->i++];
        if (b.symbol != NULL) {
            *usage = b.usage;
            return b.symbol;
        }
    }
    return NULL;
}

#endif
