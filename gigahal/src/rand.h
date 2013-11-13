
#ifndef GH_RAND_H
#define GH_RAND_H

#include "defs.h"

#include <time.h>

typedef struct {
    unsigned short xi[3];
} GhRand;

GH_AINLINE void gh_rand_init(GhRand * rand)
{
    time_t t = time(NULL);
    rand->xi[0] = 0x330e;
    rand->xi[1] = (unsigned short)t;
    rand->xi[2] = (unsigned short)((uint32_t)t >> 16);
}

GH_AINLINE double gh_rand_double(GhRand * rand)
{
    return erand48(rand->xi);
}

GH_AINLINE size_t gh_rand_int(GhRand * rand, size_t min, size_t max)
{
    return ((size_t)nrand48(rand->xi)) % (max - min) + min;
}

#endif
