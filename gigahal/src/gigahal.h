
#ifndef GH_GIGAHAL_H
#define GH_GIGAHAL_H

#include <stddef.h>
#include "rand.h"

#define GH_VERSION 2.0-alpha

typedef struct {
    float keyword_cutoff;
} GhConfig;

extern GhConfig gh_config_default;

typedef struct GhBrain {
    size_t symbol_count;
    size_t symbol_mask;
    struct GhSymbol ** symbols;

    size_t chain_count;
    size_t chain_mask;
    struct GhChain ** chains;

    size_t max_symbol_usage;

    GhConfig config;

    GhRand rand;
} GhBrain;

GhBrain * gh_new_brain(GhConfig config);
GhBrain * gh_new_brain_default(void);

void gh_input_no_reply(GhBrain * gh, const char * input);
char * gh_input_with_reply(GhBrain *gh, const char * input);

/* implemented in serialize.c */
void gh_save(GhBrain * gh, const char * path);
GhBrain * gh_load(const char * filename);

#endif
