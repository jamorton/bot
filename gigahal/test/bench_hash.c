
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

typedef unsigned int hash_t;
typedef hash_t (*hash_func_t)(char *);

static char ** words = NULL;
#define NUM_WORDS 459026

/* -------------------------------------------
   HASH FUNCTIONS
   ------------------------------------------- */
#define DWORD_HAS_ZERO_BYTE(V)       (((V) - 0x01010101UL) & ~(V) & 0x80808080UL)
#define _lrotl(x, n)        ((((unsigned long)(x)) << ((int) ((n) & 31))) | (((unsigned long)(x)) >> ((int) ((-(n)) & 31))))
static hash_t jesteress(char * str)
{
	const hash_t PRIME = 709607;
    hash_t hash32 = 2166136261;
    const char *p = str;

    for(;;)
    {
        hash_t dw1 = *(hash_t *)p;
        if ( DWORD_HAS_ZERO_BYTE(dw1) )
            break;

        p += 4;
        hash32 = hash32 ^ _lrotl(dw1,5);

        hash_t dw2 = *(hash_t *)p;
        if ( DWORD_HAS_ZERO_BYTE(dw2) )
        {
            // finish dw1 without dw2
            hash32 *= PRIME;
            break;
        }

        p += 4;

        hash32 = (hash32 ^ dw2) * PRIME;
    }

    while(*p)
    {
        hash32 = (hash32 ^ *p) * PRIME;
        p++;
    }

    return hash32;
}

static hash_t bad_hash(char * str)
{
	hash_t hash = 0, c;
	while ((c = *str++))
		hash += c * c ^ 0x03962869;
	return hash;
}

static hash_t sdbm(char * str)
{
	hash_t hash = 0, c;
	while ((c = *str++))
		hash = c + (hash << 6) + (hash << 16) - hash;
	return hash;
}

static hash_t bernstein(char * str)
{
	hash_t hash = 5381, c;
	while ((c = *str++))
		hash = ((hash << 5) + hash) + c;
	return hash;
}

static hash_t bernstein_new(char * str)
{
	hash_t hash = 5381, c;
	while ((c = *str++))
		hash = ((hash << 5) + hash) ^ c;
	return hash;
}

static hash_t fnv1a(char * str)
{
	hash_t hash = 0x811c9dc5, c;
	while ((c = *str++))
		hash = (0x01000193 * hash) ^ c;
	return hash;
}

#define rol(x, n) (((x)<<(n)) | ((x)>>(-(int)(n)&(8*sizeof(x)-1))))
#define getu32(p) (*(uint32_t *)(p))

static hash_t luajit(char * str)
{
    size_t len = strlen(str);
    hash_t a, b, h = len;
    if (len >= 4) {  /* Caveat: unaligned access! */
        a = getu32(str);
        h ^= getu32(str+len-4);
        b = getu32(str+(len>>1)-2);
        h ^= b; h -= rol(b, 14);
        b += getu32(str+(len>>2)-1);
    } else if (len > 0) {
        a = *(const uint8_t *)str;
        h ^= *(const uint8_t *)(str+len-1);
        b = *(const uint8_t *)(str+(len>>1));
        h ^= b; h -= rol(b, 14);
    } else {
        return 0;
    }
    a ^= h; a -= rol(h, 11);
    b ^= a; b -= rol(a, 25);
    h ^= b; h -= rol(b, 16);
    return h;
}

#define HASH_FUNCS_MACRO                        \
	X(bad_hash)                                 \
	X(bernstein)                                \
	X(bernstein_new)                            \
	X(sdbm)                                     \
	X(fnv1a)                                    \
	X(jesteress)                                \
    X(luajit)

#define X(a) a,
hash_func_t hash_funcs[] = {
	HASH_FUNCS_MACRO
};
#undef X

#define X(a) #a,
const char * hash_names[] = {
	HASH_FUNCS_MACRO
};
#undef X

#define NUM_HASHES (sizeof(hash_names)/sizeof(const char *))

/*----------------------------------------------------------
   HASH FUNCTION TESTER
  ----------------------------------------------------------*/

static const unsigned int buckets[] = {
    459026,
    459027,
    459072,
    393421,
    786433,
    786496,
	1572869
};
#define NUM_BUCKET_CHECKS (sizeof(buckets)/sizeof(unsigned int))

static void count_collisions(hash_func_t func)
{
	unsigned int num_cols, i, k, bkt;
	char * hashes;

	hash_t hash;

	for (k = 0; k < NUM_BUCKET_CHECKS; k++)
	{
		num_cols = 0;
		bkt = buckets[k];
		hashes = calloc(1, bkt);
		for (i = 0; i < NUM_WORDS; i++)
		{
			hash = (*func)(words[i]) % bkt;
			if (hashes[hash] == 0)
				hashes[hash] = 1;
			else
				num_cols++;
		}
		free(hashes);

		printf("%2.2f ", (double)num_cols / NUM_WORDS * 100);
	}
}

static void test_collisions(void)
{
	unsigned int i, num_cols;

	for (i = 0; i < NUM_HASHES; i++)
	{
		printf("%15s ", hash_names[i]);
        clock_t start = clock();
		count_collisions(hash_funcs[i]);
        clock_t end = clock();
        unsigned int t = end - start;
        printf("  %u\n", t);
	}
}

int main(void)
{
	FILE * fp = fopen("test/words.txt", "r");
	words = malloc(NUM_WORDS * sizeof(char *));
	char tmp[30];
	size_t sz;
	unsigned int i = 0;

	while (fgets(tmp, 30, fp) != NULL)
	{
		sz = strlen(tmp);
		words[i] = malloc(sz);
		strncpy(words[i], tmp, sz - 1); // remove trailing \n
		i++;
	}

	test_collisions();

	return 0;
}
