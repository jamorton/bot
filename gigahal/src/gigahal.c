
#include "gigahal.h"
#include "defs.h"
#include "symbol.h"
#include "syntax.h"
#include "chain.h"
#include "symbol_set.h"
#include "iqsort.h"

#include <math.h>
#include <string.h>
#include <time.h>

GhConfig gh_config_default = {
    .keyword_cutoff = 0.4f
};

/*----------------------------------------------------------
   Learning Functions
  ----------------------------------------------------------*/

GhBrain * gh_new_brain(GhConfig config)
{
    GhBrain * gh = gh_new(GhBrain);

    gh->config = config;
    gh->max_symbol_usage = 0;
    gh_symbol_init(gh);
    gh_chain_init(gh);

    gh_rand_init(&gh->rand);

    return gh;
}

GhBrain * gh_new_brain_default(void)
{
    return gh_new_brain(gh_config_default);
}

static void learn(GhBrain * gh, GhSvec * symbols)
{
    /* must have at least 4 symbols in an input to learn from it
       (includes null symbols on either end) */
    if (symbols->len < 6)
        return;

    size_t max = symbols->len - 4;
    GhSymbol ** array = symbols->array;

    for (size_t i = 1; i < max; i++) {

        /* quad is 4 symbol pointers, so cast directly to struct to capture
           symbols[i ... i+3] */
        GhQuad q = *((GhQuad *)(array + i));

        GhChain * chain = gh_chain_add(gh, &q);
        chain->usage++;

        gh_sset_add(&chain->back, array[i - 1]);
        gh_sset_add(&chain->forw, array[i + 4]);

        for (size_t j = 0; j < 4; j++)
            if (gh_symbol_isword(array[i + j]))
                gh_symbol_add_chain(array[i + j], chain);

        gh_symbol_inc_usage(gh, array[i]);
    }

    gh_symbol_inc_usage(gh, array[max  ]);
    gh_symbol_inc_usage(gh, array[max+1]);
    gh_symbol_inc_usage(gh, array[max+2]);
}

static GhSvec * get_symbols(GhBrain * gh, const char * input, size_t len)
{
    GhSvec * symbols = gh_svec(); //_s(len > 2 ? len/3 : 3);

    /* prepend and append input with null symbols, we use these
       while generating output to know when to stop */
    gh_svec_push(symbols, GH_SYMBOL_NONE);
    gh_parse_symbols(gh, symbols, input);
    gh_svec_push(symbols, GH_SYMBOL_NONE);

    return symbols;
}

void gh_input_no_reply(GhBrain * gh, const char * input)
{
    GhSvec * symbols = get_symbols(gh, input, strlen(input));
    learn(gh, symbols);
    gh_svec_del(symbols);
}

/*----------------------------------------------------------
   Replying Functions
  ----------------------------------------------------------*/

#ifdef GH_DEBUG
#define reply_debug(...) printf(__VA_ARGS__)
#else
#define reply_debug(...)
#endif

GH_AINLINE void print_chain(GhChain * chain)
{
    reply_debug("('%s', '%s', '%s', '%s') [%d]",
        chain->q.a->data, chain->q.b->data,
        chain->q.c->data, chain->q.d->data,
        chain->usage);
}

#define MAX_SYMBOLS 150

/* stores all state to be passed around to various reply functions */
typedef struct {
    GhBrain * gh;

    const char * input; /* original input string */
    GhSvec * symbols;   /* original input string (parsed into a symbol list) */

    GhSymbolSet * keywords;
    GhSymbol * primary_keyword;
    double kw_avg_usage;

    GhChain * initial_chain;

    GhSymbol * buffer[MAX_SYMBOLS * 2 + 2];
    size_t num_symbols;

    size_t total_len;
} ReplyContext;

#define rand_double(ctx) (gh_rand_double(&(ctx)->gh->rand))
#define rand_int(ctx, min, max) (gh_rand_int(&(ctx)->gh->rand, min, max))

GH_AINLINE void buffer_add_forw(ReplyContext * ctx, GhSymbol * symbol)
{
    ctx->buffer[MAX_SYMBOLS + ctx->num_symbols++] = symbol;
    ctx->total_len++;
}

GH_AINLINE void buffer_add_back(ReplyContext * ctx, GhSymbol * symbol)
{
    ctx->buffer[MAX_SYMBOLS - ctx->num_symbols++ - 1] = symbol;
    ctx->total_len++;
}

static GhSymbol * random_keyword(ReplyContext * ctx)
{
    size_t i = rand_int(ctx, 0, ctx->gh->symbol_mask + 1),
        max = i + ctx->gh->symbol_mask + 1;
    for (; i < max; i++) {
        GhSymbol * symbol = ctx->gh->symbols[i & ctx->gh->symbol_mask];
        while (symbol != NULL) {
            if (symbol->usage > 0 && symbol->chp_len > 0)
                return symbol;
            symbol = symbol->next;
        }
    }
    return NULL;
}

/**
 * get_keywords
 *
 * fills ctx->keywords with any symbols that are acceptable keywords
 * and selects a primary keyword from this set
 */
static void get_keywords(ReplyContext * ctx)
{
    reply_debug("Keywords: ");

    GhSymbol * slist[ctx->symbols->len];
    size_t slen = 0;

    double total_usage = 0;

    GH_SVEC_FOREACH(ctx->symbols, symbol) {

        if (!gh_symbol_isword(symbol) ||
            symbol->usage == 0 ||
            symbol->chp_len == 0)
            continue;

        if (gh_sset_add(ctx->keywords, symbol)) {
            total_usage += symbol->usage;
            reply_debug("%s (%u), ", symbol->data, symbol->usage);
            slist[slen++] = symbol;
            symbol->flags &= ~GH_SYMBOL_USED;
        }
    }

    reply_debug("\n");

    if (ctx->keywords->len == 0)
        return;

    /* sort potential keywords by their usage (ascending) */
#define CMP_L(a, b) ((*a)->usage < (*b)->usage)
    QSORT(GhSymbol *, slist, slen, CMP_L);
#undef CMP_L

    ctx->kw_avg_usage = total_usage / (double)slen;
    reply_debug("Average usage: %f\n", total_usage / (double)slen);

    /* Don't consider keywords with the higher usages */
     if (slen >= 6)
        slen /= 2;
    else if (slen >= 3)
        slen = slen * 2 / 3;

    /* choose a primary keyword, favoring more unique words */
    double choice = (1 - sqrt(rand_double(ctx))) * slen;
    ctx->primary_keyword = slist[(size_t)choice];
}

/**
 * Using the chosen primary_keyword, choose a starting chain
 */
static void choose_chain(ReplyContext * ctx)
{
    GhSymbol * key = ctx->primary_keyword;
    size_t offs = rand_int(ctx, 0, key->chp_mask + 1);

    for (size_t i = key->chp_mask; i != ~(size_t)0; --i) {
        GhChain * chain = key->chp[(i + offs) & key->chp_mask];
        if (chain == NULL)
            continue;

        ctx->initial_chain = chain;
        return;
    }

    gh_panic("Failed to choose an initial chain");
}

/**
 *
 */
GH_AINLINE GhChain * next_chain(ReplyContext * ctx, GhChain * chain, bool forw)
{
    GhSymbolSet * set = forw ? &chain->forw : &chain->back;
    size_t i = rand_int(ctx, 0, set->mask + 1);

    GhSymbol * next = NULL;

    double inv = 1 / (double)set->len;

    for (size_t max = i + set->mask + 1; i < max; i++) {
        GhSymbol * symb = set->table[i & set->mask].symbol;
        if (!symb)
            continue;

        if (!(symb->flags & GH_SYMBOL_USED) &&
              gh_sset_has(ctx->keywords, symb) &&
              symb->usage < ctx->kw_avg_usage &&
              rand_double(ctx) < 0.8) {
            reply_debug("Choosing kw '%s'\n", symb->data);
            next = symb;
            next->flags |= GH_SYMBOL_USED;
            break;
        }

        if (set->len == 1) {
            next = symb;
            break;
        }

        if (symb == GH_SYMBOL_NONE) {
            /* if we see a termiating symbol, we prefer not to pick it if
               our output has too few words */
            if (ctx->num_symbols < 8 && rand_double(ctx) < ctx->num_symbols / 16.0)
                next = GH_SYMBOL_NONE;
        } else if (next == NULL)
            next = symb;

        /* if this symbol has a lower usage than the one we currently have
           picked out, we want to prefer picking this one (according to some
           variable chance) */
        else if (symb->usage < next->usage) {
            double chance = (1.5*(1.0 - symb->usage / (double)next->usage) + inv)/2.5;
            reply_debug("'%s' (%d) vs '%s' (%d) chance %f\n", symb->data, symb->usage, next->data, next->usage, chance);
            if (rand_double(ctx) < chance)
                next = symb;

        /* otherwise, each symbol has equal chance 1/len of being picked */
        } else if (rand_double(ctx) < inv)
            next = symb;
    }

    if (!next || next == GH_SYMBOL_NONE)
        return NULL;

    if (forw) {
        GhQuad q = {chain->q.b, chain->q.c, chain->q.d, next};
        return gh_chain_get(ctx->gh, &q);
    } else {
        GhQuad q = {next, chain->q.a, chain->q.b, chain->q.c};
        return gh_chain_get(ctx->gh, &q);
    }
}

char * gh_input_with_reply(GhBrain * gh, const char * input)
{
    ReplyContext ctx;
    GhChain * chain;

    /* set up the reply context */
    ctx.gh = gh;
    ctx.input = input;
    ctx.symbols = get_symbols(gh, input, strlen(input));
    ctx.keywords = gh_sset_new();
    ctx.primary_keyword = NULL;
    ctx.total_len = 0;

    /* find potential keywords and choose a primary one */
    get_keywords(&ctx);

    if (ctx.primary_keyword == NULL)
        ctx.primary_keyword = random_keyword(&ctx);
    if (ctx.primary_keyword == NULL) {
        const char * resp = "I don't know enough to answer yet.";
        size_t len = strlen(resp);
        char * ret = gh_falloc(len + 1);
        memcpy(ret, resp, len + 1);

        learn(gh, ctx.symbols);

        gh_sset_del(ctx.keywords);
        gh_svec_del(ctx.symbols);

        return ret;
    }

    reply_debug("Primary keyword: %s\n", ctx.primary_keyword->data);
    ctx.primary_keyword->flags |= GH_SYMBOL_USED;

    /* choose our initial chain */
    choose_chain(&ctx);
    reply_debug("Initial chain: ");
    print_chain(ctx.initial_chain);
    reply_debug("\n");

    /* generate chains forward from the initial chain */
    ctx.num_symbols = 0;
    chain = ctx.initial_chain;
    buffer_add_forw(&ctx, chain->q.c);

    while (chain != NULL) {
        buffer_add_forw(&ctx, chain->q.d);
        chain = next_chain(&ctx, chain, true);
    }

    /* generate chains backward from the initial chain */
    ctx.num_symbols = 0;
    chain = ctx.initial_chain;
    buffer_add_back(&ctx, chain->q.b);

    while (chain != NULL) {
        buffer_add_back(&ctx, chain->q.a);
        chain = next_chain(&ctx, chain, false);
    }

    size_t start = MAX_SYMBOLS - ctx.num_symbols;
    char * out = gh_join_symbols(gh, &ctx.buffer[start], ctx.total_len);

    /* finally, after we've generated a reply, we learn from the input */
    learn(gh, ctx.symbols);

    gh_sset_del(ctx.keywords);
    gh_svec_del(ctx.symbols);

    return out;
}
