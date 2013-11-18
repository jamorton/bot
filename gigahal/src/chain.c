
#include "chain.h"
#include "symbol_set.h"

#include <stdio.h>

#define CHAIN_FOREACH(ch) \
    for (; ch != NULL; ch = ch->next)

GH_AINLINE void resize_table(GhBrain * gh)
{
    size_t new_mask = (gh->chain_mask << 1) + 1;
    GhChain ** new_chains = gh_new_array(GhChain *, new_mask + 1);

    for (size_t i = gh->chain_mask; i != ~(size_t)0; --i) {
        GhChain * chain = gh->chains[i];
        while (chain != NULL) {
            size_t bucket = chain->hash & new_mask;
            GhChain * next = chain->next;
            chain->next = new_chains[bucket];
            new_chains[bucket] = chain;
            chain = next;
        }
    }

    gh_free(gh->chains);
    gh->chains = new_chains;
    gh->chain_mask = new_mask;
}

GH_AINLINE void check_resize(GhBrain * gh)
{
    /* allow up to 100% load factor before resizing */
    if (gh_unlikely(gh->chain_count > gh->chain_mask))
        resize_table(gh);
}

GH_AINLINE GhChain * alloc_chain()
{
    GhChain * chain = gh_new(GhChain);
    gh_sset_init(&chain->forw, 1);
    gh_sset_init(&chain->back, 1);
    return chain;
}

void gh_chain_init(GhBrain * gh)
{
    gh->chain_count = 0;
    gh->chain_mask = 0xF;
    gh->chains = gh_new_array(GhChain *, gh->chain_mask + 1);
}

void gh_chain_deinit(GhBrain * gh)
{
    for (size_t i = 0; i <= gh->chain_mask; i++) {
        GhChain * chain = gh->chains[i];
        while (chain != NULL) {
            GhChain * next = chain->next;
            gh_free(chain->forw.table);
            gh_free(chain->back.table);
            gh_free(chain);
            chain = next;
        }
    }
    gh_free(gh->chains);
}

GhChain * gh_chain_get(GhBrain * gh, GhQuad * q)
{
    GhChain * chain = gh->chains[gh_hash_quad(q) & gh->chain_mask];

    CHAIN_FOREACH(chain)
        if (gh_quad_eq(q, &chain->q))
            return chain;

    return NULL;
}

GhChain * gh_chain_add(GhBrain * gh, GhQuad * q)
{
    hash_t hash = gh_hash_quad(q);
    size_t bucket = hash & gh->chain_mask;
    GhChain * chain = gh->chains[bucket];

    CHAIN_FOREACH(chain)
        if (gh_quad_eq(q, &chain->q))
            return chain;

    chain = alloc_chain();

    chain->hash = hash;
    chain->q = *q;
    chain->next = gh->chains[bucket];
    gh->chains[bucket] = chain;

    gh->chain_count++;

    check_resize(gh);

    return chain;
}
