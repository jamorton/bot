
#include "syntax.h"

#include <string.h>

#define C_SKIP    1
#define C_NONE    2
#define C_NONWORD 4
#define C_WORD    8

/* generated by tools/gen_charmap.py */
static const uint8_t charmap[] = {C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_NONWORD, C_NONWORD, C_NONWORD, C_NONE, C_NONE, C_NONE, C_NONWORD, C_NONE, C_NONWORD, C_NONWORD, C_NONWORD, C_NONWORD, C_NONWORD, C_NONE, C_NONWORD, C_NONWORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_NONWORD, C_NONWORD, C_NONWORD, C_NONWORD, C_NONWORD, C_NONWORD, C_NONE, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_NONWORD, C_NONWORD, C_NONWORD, C_NONWORD, C_NONE, C_NONWORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_WORD, C_NONWORD, C_NONWORD, C_NONWORD, C_NONWORD, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP, C_SKIP};

void gh_parse_symbols(GhBrain * gh, GhSvec * vec, const char * str)
{
    char c;
    bool on_word = false;
    const char * iter = str;

    /* figure out if the first symbol is going to be a
       word or a non-word */
    while ((c = *iter++)) {
        uint8_t a = charmap[(uint8_t)c];
        if (a == C_NONWORD)
            break;
        if (a == C_WORD) {
            on_word = true;
            break;
        }
    }

    char symbuf[GH_SYMBOL_MAXLEN];
    size_t len = 0;
    iter = str;

    while ((c = *iter++)) {

        if (c > 64 && c < 91)
            c += 32;

        uint8_t type = charmap[(uint8_t)c];
        /* any invalid characters are converted to spaces */
        if (type & C_SKIP)
            c = ' ';

        /* detect word-nonword boundary and split off a new
           symbol */
        if ((type & C_WORD && !on_word) ||
            (type & C_NONWORD && on_word)) {

            uint8_t flags = on_word ? GH_SYMBOL_ISWORD : 0;
            gh_svec_push(vec, gh_symbol_newl(gh, symbuf, len, flags));
            symbuf[0] = c;
            len = 1;
            on_word = !on_word;
        } else {
            /* prevent multiple spaces in a row */
            if (gh_likely(len < GH_SYMBOL_MAXLEN &&
                    (c != ' ' || len < 1 || symbuf[len - 1] != ' ')))
                symbuf[len++] = c;
        }
    }

    if (len == 0)
        return;

    GhSymbol * last = gh_symbol_newl(gh, symbuf, len, on_word ? GH_SYMBOL_ISWORD : 0);
    gh_svec_push(vec, last);

    /* if the last symbol is not punctuation, add a period, so all our
       sentences end in one */
    if (gh_symbol_isword(last))
        gh_svec_push(vec, gh_symbol_newl(gh, ".", 1, 0));
}

/**
 * Given a list of symbols, join them together into a string
 * with (hopefully) proper punctuation, capitalization, etc.
 */
char * gh_join_symbols(GhBrain * gh, GhSymbol ** symbols, size_t len)
{
    size_t i, tot_len = 0;

    /* count total number of characters in all symbols */
    for (i = 0; i < len; i++)
        tot_len += symbols[i]->len;

    char buf[tot_len + 1];
    size_t chars = 0, nquote = 0;

    bool cap_next = false;

    /* loop through each symbol, adding each character to a temporary
       buffer for more processing.
       also do some symbol-level fixup stuff */
    for (i = 0; i < len; i++) {
        char c, * s = symbols[i]->data;

        /* capitalize "i" and "i'_" (e.g. I'm). */
        if (s[0] == 'i' && (s[1] == '\'' || symbols[i]->len == 1)) {
            buf[chars++] = 'I';
            s++;
        }
        /* capitalize the first letter after .!? */
        else if (cap_next && *s >= 'a' && *s <= 'z') {
            buf[chars++] = *s - 32;
            s++;
        }

        cap_next = false;

        while ((c = *s++)) {
            if (c == '"')
                nquote++;
            else if (c == '.' || c == '?' || c == '.')
                cap_next = true;
            buf[chars++] = c;
        }
    }

    buf[tot_len] = '\0';

    /* if there are an odd number of ", remove one of them */
    int remove_quote = nquote & 1;
    size_t final_len = chars - remove_quote;
    char * out = gh_falloc(final_len + 1);
    bool first_letter = false;
    size_t j = 0;

    /* after calculating the final output length after modifications,
       loop one more time copying to the final output string,
       and do some character-level processing */
    for (i = 0; i < chars; i++) {
        char c = buf[i];

        if (c == '"' && remove_quote) {
            remove_quote = 0;
            continue;
        }

        /* capitalize first letter */
        if (!first_letter) {
            if (c >= 'a' && c <= 'z') {
                c -= 32;
                first_letter = true;
            } else if (c >= 'A' && c <= 'Z')
                first_letter = true;
        }

        out[j++] = c;
    }

    out[j] = '\0';
    return out;
}