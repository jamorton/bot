
#include "test.h"
#include "symbol_set.h"
#include "chain.h"

static void learn_all(GhBrain * gh)
{
	FILE * fp = fopen("data/skype.txt", "r");
    char buf[10000];

    while (fgets(buf, sizeof(buf), fp) != NULL) {
        size_t sz = strlen(buf);
        if (buf[sz - 1] == '\n')
            sz--;
        if (buf[sz - 1] == '\r')
            sz--;

        buf[sz] = '\0';

        gh_input_no_reply(gh, buf);
    }
    fclose(fp);
}

int main(int argc, const char * argv[])
{
    double mb;
    for (;;) {
        GhBrain * gh = gh_new_brain();
        learn_all(gh);
        gh_save(gh, "stress.tmp.gh");
        gh_brain_del(gh);

        gh = gh_load("stress.tmp.gh");
        remove("stress.tmp.gh");

        gh_brain_del(gh);

        mb = gh_mem_total_bytes()/1024.0/1024.0;
        printf("Memory usage: %.2f (should be 0.00)\n", mb);
    }
}
