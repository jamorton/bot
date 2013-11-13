
#include "gigahal.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct {
    size_t len;
    char ** words;
} WordList;

static inline WordList load_words(const char * filename)
{
    size_t max = 32;

    WordList list;
    list.len = 0;
    list.words = (char **)malloc(sizeof(char *) * max);

    char buf[200];
	FILE * fp = fopen(filename, "r");

    while (fgets(buf, 200, fp) != NULL) {
        size_t sz = strlen(buf);
        if (buf[sz - 1] == '\n')
            sz--;
        if (buf[sz - 1] == '\r')
            sz--;

        char * mem = (char *)malloc(sz);
        memcpy(mem, buf, sz + 1);
        mem[sz] = '\0';

        if (list.len >= max) {
            max <<= 1;
            list.words = (char **)realloc(list.words, sizeof(char *) * max);
        }

        list.words[list.len++] = mem;
    }

    return list;
}

static char * readline(void)
{
    char * line = malloc(100), * linep = line;
    size_t lenmax = 100, len = lenmax;
    int c;

    if(line == NULL)
        return NULL;

    for(;;) {
        c = fgetc(stdin);
        if(c == EOF || c == '\n')
            break;

        if(--len == 0) {
            len = lenmax;
            char * linen = realloc(linep, lenmax *= 2);

            if(linen == NULL) {
                free(linep);
                return NULL;
            }
            line = linen + (line - linep);
            linep = linen;
        }

        *line++ = c;
    }
    *line = '\0';
    return linep;
}
