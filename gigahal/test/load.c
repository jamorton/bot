
#include "test.h"
#include "symbol_set.h"
#include "chain.h"

int main(int argc, const char * argv[])
{

    GhBrain * gh = gh_new_brain();

	FILE * fp = fopen("data/skype.txt", "r");
    char buf[10000];
    size_t i = 0;
    clock_t start = clock();

    while (fgets(buf, sizeof(buf), fp) != NULL) {

        size_t sz = strlen(buf);
        if (buf[sz - 1] == '\n')
            sz--;
        if (buf[sz - 1] == '\r')
            sz--;

        buf[sz] = '\0';

        gh_input_no_reply(gh, buf);

        i++;
    }

    size_t cutoff = (size_t)((float)gh->max_symbol_usage * 0.4f);

    printf("%zu\n", cutoff);

    for (size_t j = 0; j <= gh->symbol_mask; j++) {
        GhSymbol * symbol = gh->symbols[j];
        while (symbol != NULL) {
            if (symbol->usage >= cutoff)
                printf("%s (%u)\n", symbol->data, symbol->usage);
            symbol = symbol->next;
        }
    }

    double time = (clock() - start) / (double)CLOCKS_PER_SEC;
    double ips = i / time;

    printf("\n");

    printf("...Done loading %zu inputs.\n", i);
    printf("Took %.2f seconds (%.2f inputs per second)\n", time, ips);

    printf("\n%zu symbols, %zu chains\n", gh->symbol_count, gh->chain_count);

    size_t bytes = gh_mem_total_bytes();
    double mb = ((double)bytes)/1024.0/1024.0;
    printf("%zu bytes allocated (%.2f mb)\n", bytes, mb);

    printf("\n");

    fclose(fp);
}
