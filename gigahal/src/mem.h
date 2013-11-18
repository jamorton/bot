
#ifndef GH_MEM_H
#define GH_MEM_H

#include <stddef.h>

// Make struct allocation look pretty
#define gh_new(obj) ((obj *)gh_alloc(sizeof(obj)))
#define gh_new_array(obj, count) ((obj *)gh_alloc(sizeof(obj) * (count)))
#define gh_delete(ptr) (gh_free(ptr))

void * gh_alloc(size_t sz); /* allocate zeroed memory */
void * gh_falloc(size_t sz); /* allocate memory without zeroing */
void * gh_realloc(void * ptr, size_t sz);
void   gh_free(void * ptr);

size_t gh_mem_total_bytes(void);

#endif
