
#ifndef GH_DEFS_H
#define GH_DEFS_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

typedef uint32_t hash_t;

#define GH_AINLINE static inline __attribute__((always_inline))

GH_AINLINE size_t gh_randint(size_t min, size_t max)
{
    return (((size_t)rand()) % (max - min)) + min;
}

GH_AINLINE float gh_randfloat(void)
{
    return rand() / ((float)RAND_MAX);
}

/* asserts on only during debug */
#ifdef GH_DEBUG
#define gh_assert(c) assert(c)
#else
#define gh_assert(c)
#endif

#define gh_panic(m)                             \
    do {                                        \
        printf("Gigahal PANIC!\n  ");           \
        printf(m);                              \
        printf("\n");                           \
        abort();                                \
    } while(0)                                  \

#define gh_likely(x)    __builtin_expect(!!(x), 1)
#define gh_unlikely(x)  __builtin_expect(!!(x), 0)

#include "mem.h"

#endif
