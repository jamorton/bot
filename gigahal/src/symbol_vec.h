
/**
 * GhSvec: symbol vector objects
 * (dynamic arrays of GhSymbol)
 */

#ifndef GH_SYMBOL_VEC_H
#define GH_SYMBOL_VEC_H

#include "defs.h"
#include "symbol.h"

typedef struct {
    GhSymbol ** array;
    size_t len;
    size_t max;
} GhSvec;

#define GH_SVEC_INITIAL_SIZE 8

GH_AINLINE GhSvec * gh_svec_s(size_t initial_size)
{
    GhSvec * ret = gh_new(GhSvec);
    ret->max = initial_size;
    ret->len = 0;
    ret->array = gh_new_array(GhSymbol *, initial_size);
    return ret;
}

GH_AINLINE GhSvec * gh_svec(void)
{
    return gh_svec_s(GH_SVEC_INITIAL_SIZE);
}

GH_AINLINE void gh_svec_del(GhSvec * vec)
{
    gh_free(vec->array);
    gh_free(vec);
}

GH_AINLINE void gh_svec_push(GhSvec * vec, GhSymbol * el)
{
    if (vec->len >= vec->max) {
        vec->max <<= 1;
        vec->array = gh_realloc(vec->array, sizeof(GhSymbol *) * vec->max);
    }
    vec->array[vec->len++] = el;
}

GH_AINLINE bool gh_svec_contains(GhSvec * vec, GhSymbol * el)
{
    for (size_t i = 0; i < vec->len; i++)
        if (gh_symbol_eq(el, vec->array[i]))
            return true;
    return false;
}

#define GH_SVEC_FOREACH(v, e)                                       \
    for (size_t _i = 0, _j = 0; _i < v->len; _i++)     \
        for (GhSymbol * e = v->array[(_j = 0) + _i]; _j < 1; _j++)

#endif
