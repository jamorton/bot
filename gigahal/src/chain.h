
#ifndef GH_CHAIN_H
#define GH_CHAIN_H

#include "defs.h"
#include "gigahal.h"
#include "symbol.h"
#include "symbol_set.h"

/**
 * GhQuad holds a list of 4 symbols (for order-4 markov models)
 */
typedef struct GhQuad {
    GhSymbol * a;
    GhSymbol * b;
    GhSymbol * c;
    GhSymbol * d;
} GhQuad;

typedef struct GhChain {
    GhQuad q;
    GhSymbolSet forw;
    GhSymbolSet back;
    struct GhChain * next;
    uint32_t usage;
    hash_t hash;
} GhChain;

GH_AINLINE bool gh_quad_eq(GhQuad * x, GhQuad * y)
{
    return (
        x->a == y->a &&
        x->b == y->b &&
        x->c == y->c &&
        x->d == y->d
    );
}

void gh_chain_init(GhBrain * gh);

GhChain * gh_chain_get(GhBrain * gh, GhQuad * q);
GhChain * gh_chain_add(GhBrain * gh, GhQuad * q);

GH_AINLINE hash_t gh_hash_quad(GhQuad * q)
{
    return (
        (((q->a->hash  * 37)
         + q->b->hash) * 37
         + q->c->hash) * 37
         + q->d->hash);
}

#endif
