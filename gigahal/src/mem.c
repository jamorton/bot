
#include "mem.h"

#include <stdlib.h>

#ifdef __APPLE__
#include <malloc/malloc.h>
#elif __linux__
#include <malloc.h>
#define malloc_size malloc_usable_size
#endif

static size_t bytes_used;
static size_t total_allocs;
static size_t active_allocs;

void * gh_alloc(size_t sz)
{
    total_allocs++;
    active_allocs++;
	void * ret = calloc(1, sz);
    bytes_used += malloc_size(ret);
    return ret;
}

void * gh_falloc(size_t sz)
{
    total_allocs++;
    active_allocs++;
    void * ret = malloc(sz);
    bytes_used += malloc_size(ret);
    return malloc(sz);
}

void * gh_realloc(void * ptr, size_t sz)
{
    total_allocs++;
    bytes_used -= malloc_size(ptr);
    ptr = realloc(ptr, sz);
    bytes_used += malloc_size(ptr);
    return ptr;
}

void gh_free(void * ptr)
{
    active_allocs--;
    bytes_used -= malloc_size(ptr);
	free(ptr);
}

size_t gh_mem_total_bytes()
{
    return bytes_used;
}
