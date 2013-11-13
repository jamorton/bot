

#include "test.h"
#include "symbol_set.h"
#include "chain.h"

int main(int argc, const char * argv[])
{
    GhBrain * gh = gh_new_brain(gh_config_default);

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

    for (;;) {

        putchar('>');
        char * line = readline();

        if (strcmp(line, "!q") == 0) {
            free(line);
            break;
        }

        char * out = gh_input_with_reply(gh, line);
        printf("%s\n", out);

        gh_free(out);
        free(line);
    }
}
