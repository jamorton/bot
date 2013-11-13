
#include "symbol_set.h"

#include <stdio.h>

GH_AINLINE GhSetBucket * find_bucket(GhSymbolSet * set, GhSymbol * symbol)
{
    for (size_t i = 0; i <= set->mask; i++) {

        size_t bucket = (symbol->hash + i) & set->mask;
        GhSymbol * cur = set->table[bucket].symbol;

        if (cur == NULL || gh_symbol_eq(symbol, cur))
            return &set->table[bucket];
    }

    /* no empty bucket found, should never happen... */
    gh_panic("SymbolSet: failed to find bucket");
}

GH_AINLINE void resize_table(GhSymbolSet * set)
{
    GhSetBucket * old = set->table;
    size_t old_mask = set->mask;
    set->mask = (set->mask << 1) + 1;
    set->table = gh_new_array(GhSetBucket, set->mask + 1);
    for (size_t i = 0; i <= old_mask; i++) {
        GhSymbol * copy = old[i].symbol;
        if (copy != NULL)
            *find_bucket(set, copy) = old[i];
    }
    gh_free(old);
}

GH_AINLINE void check_resize(GhSymbolSet * set)
{
    /* allow up to 100% load factor before resizing */
    if (gh_unlikely(set->len > set->mask))
        resize_table(set);
}

GhSymbolSet * gh_sset_new()
{
    GhSymbolSet * set = gh_new(GhSymbolSet);
    gh_sset_init(set, 7);
    return set;
}

void gh_sset_init(GhSymbolSet * set, uint32_t initial_mask)
{
    set->mask = initial_mask;
    set->len  = 0;
    set->table = gh_new_array(GhSetBucket, set->mask + 1);
}

void gh_sset_del(GhSymbolSet * set)
{
    gh_free(set->table);
    gh_free(set);
}

bool gh_sset_has(GhSymbolSet * set, GhSymbol * symbol)
{
    return find_bucket(set, symbol)->symbol == symbol;
}

bool gh_sset_add(GhSymbolSet * set, GhSymbol * symbol)
{
    GhSetBucket * bucket = find_bucket(set, symbol);

    if (bucket->symbol == NULL) {
        bucket->usage = 1;
        bucket->symbol = symbol;
        set->len++;
        check_resize(set);
        return true;
    }

    bucket->usage++;
    return false;
}

void gh_sset_set_usage(GhSymbolSet * set, GhSymbol * symbol, size_t usage)
{
    GhSetBucket * b = find_bucket(set, symbol);
    if (b)
        b->usage = usage;
}
