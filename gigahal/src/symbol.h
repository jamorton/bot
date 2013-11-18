
#ifndef GH_SYMBOL_H
#define GH_SYMBOL_H

#include "defs.h"
#include "gigahal.h"

/* symbol flags */
#define GH_SYMBOL_ISWORD 1
#define GH_SYMBOL_USED 2

#define GH_SYMBOL_MAXLEN (UINT16_MAX-2)

typedef struct GhSymbol {
    struct GhSymbol * next;
    hash_t hash;
    uint32_t usage;

    /* each symbol stores a set of pointers to GhChains
       that it is a part of. This is so we can get a 4-symbol
       progression given only a keyword symbol */
    struct GhChain ** chp;
    uint32_t chp_mask;
    uint32_t chp_len;

    uint16_t len;
    uint8_t flags;

    /* more space is allocated after struct for entire string,
       the `data` member is actually is variable length */
    char data[1];
} GhSymbol;

extern GhSymbol gh_null_symbol;
#define GH_SYMBOL_NONE (&gh_null_symbol)

GH_AINLINE bool gh_symbol_eq(GhSymbol * a, GhSymbol * b)
{
    return a == b;
}

GH_AINLINE const char * gh_symbol_data(GhSymbol * symbol)
{
    return symbol->data;
}

GH_AINLINE size_t gh_symbol_len(GhSymbol * symbol)
{
    return (size_t)symbol->len;
}

GH_AINLINE bool gh_symbol_isword(GhSymbol * symbol)
{
    return symbol->flags & GH_SYMBOL_ISWORD;
}

GH_AINLINE void gh_symbol_inc_usage(GhBrain * gh, GhSymbol * symbol)
{
    symbol->usage++;
    if (symbol->flags & GH_SYMBOL_ISWORD && symbol->usage > gh->max_symbol_usage)
        gh->max_symbol_usage = symbol->usage;
}

void gh_symbol_init(GhBrain * gh);
void gh_symbol_deinit(GhBrain * gh);

GhSymbol * gh_symbol_new(GhBrain * gh, const char * str, uint8_t flags);
GhSymbol * gh_symbol_newl(GhBrain * gh, const char * str, size_t len, uint8_t flags);

void gh_symbol_add_chain(GhSymbol * symbol, struct GhChain * chain);

/**
 * left circular shift
 * Every half-decent C compiler transforms this into a rotate instruction.
 * From LuaJIT source (http://repo.or.cz/w/luajit-2.0.git/blob/HEAD:/src/lj_def.h)
 */
#define gh_rol(x, n) (((x)<<(n)) | ((x)>>(-(int)(n)&(8*sizeof(x)-1))))
#define gh_getu32(p) (*(uint32_t *)(p))

/**
 * fast, well-distributed string hash
 * from LuaJIT 2.0 source (by Mike Pall)
 * (http://repo.or.cz/w/luajit-2.0.git/blob/HEAD:/src/lj_str.c)
 */
GH_AINLINE hash_t gh_hash_string(const char * str, uint32_t len)
{
    hash_t a, b, h = len;
    if (len >= 4) {  /* Caveat: unaligned access! */
        a = gh_getu32(str);
        h ^= gh_getu32(str+len-4);
        b = gh_getu32(str+(len>>1)-2);
        h ^= b; h -= gh_rol(b, 14);
        b += gh_getu32(str+(len>>2)-1);
    } else if (len > 0) {
        a = *(const uint8_t *)str;
        h ^= *(const uint8_t *)(str+len-1);
        b = *(const uint8_t *)(str+(len>>1));
        h ^= b; h -= gh_rol(b, 14);
    } else
        return 0;
    a ^= h; a -= gh_rol(h, 11);
    b ^= a; b -= gh_rol(a, 25);
    h ^= b; h -= gh_rol(b, 16);
    return h;
}

#endif
