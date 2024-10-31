#include "freq.internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#include "eprintf.h"
#include "eread.h"
#include "hash.h"
#include "qsort.h"

const char *ErrOptReqsArg       = "-%c requires an argument";
const char *ErrOptMutex         = "-%c cannot be used with -%c";
const char *ErrOptMutexDefault  = "-R needs arg unless used with a type opt";
const char *ErrDupOptArg        = "-%c cannot be used with more than one arg";
const char *ErrMultiSizeRaw     = "-R cannot be used with multiple types/args";
const char *ErrInvOpt           = "invalid option %s";
const char *ErrInvSizeArg       = "SIZE: an integer > 0. Given SIZE: %s"; 
const char *UsageInfoStr = 
"Usage: freq [-acdDhiRsS] [-D DELIM] [-R[SIZE]] [--delim=DELIM] [--raw[=SIZE]]"
                                                               "  [file ...]\n"
"Options:\n"
"\t-h,        --help         Display this menu and exit\n"
"\t-a,        --aggregate    Aggregate input files into a combined output\n"
"\t-s,        --sort         Sort the output\n"
"\t-D DELIM,  --delim=DELIM  Set delimiter for non-raw input (regex)\n"
"\t-R[SIZE],  --raw[=SIZE]   Interpret input as a raw byte stream. Infers SIZE"
                             " from a type opt, or SIZE argument SIZE > 0\n"
"\t-c,        --char         Interpret input as ASCII chars\n"
"\t-i,        --int          Interpret input as integers\n"
"\t-d,        --double       Interpret input as doubles\n"
"\t-S,        --string       Interpret input as strings";

// struct option: defined in getopt.h, see "man 3 getopt_long"
const struct option LongOpts[] = 
{
    {"help",      no_argument,        0,  'h'},
    {"aggregate", no_argument,        0,  'a'},
    {"sort",      no_argument,        0,  's'},
    {"delim",     required_argument,  0,  'D'},
    {"raw",       optional_argument,  0,  'R'},
    {"char",      no_argument,        0,  'c'},
    {"int",       no_argument,        0,  'i'},
    {"double",    no_argument,        0,  'd'},
    {"string",    no_argument,        0,  'S'},
    {0, 0, 0, 0}
};

//  parse_opts helpers
void  e_usage           (void);
void  e_set_delim       (char **delim, int *n_delim);
void  e_set_rawsize     (int *rawsize, int *n_raw);
void  e_invalid_opt     (char **argv);
void  e_validate        (int opts, int n_raw, int n_delim);

//  key getters for generic Hashmap 
void  *get_int_freq_key (Hashmap *hmap, void *data);
void  *get_dbl_freq_key (Hashmap *hmap, void *data);
void  *get_str_freq_key (Hashmap *hmap, void *data);

//  comparison functions for qsort_generic
int   cmp_ch_freq       (void *, void *);
int   cmp_int_freq      (void *, void *);
int   cmp_dbl_freq      (void *, void *);
int   cmp_str_freq      (void *, void *);

//  Hashmap iterator callback fns that add mapped Items to an arr for sorting
void  add_int_freqs     (Item *item, int idx, va_list args);
void  add_dbl_freqs     (Item *item, int idx, va_list args);
void  add_str_freqs     (Item *item, int idx, va_list args);

Ctx *init_freq_ctx(int opts, int rawsize)
{
    Ctx *ctx = (Ctx *) ecalloc(1, sizeof(Ctx));
    
    // initialize the hashmaps for mapped types only if that type is selected
    if (opts & INT_OPT_MASK)
        ctx->type_maps[INT_MAP] = init_hmap(get_int_freq_key, sizeof(int), 
                                                        0, 0, 0, 0, 0);
    if (opts & DOUBLE_OPT_MASK)
        ctx->type_maps[DBL_MAP] = init_hmap(get_dbl_freq_key, sizeof(double), 
                                                              0, 0, 0, 0, 0);
    // mutex options, if: RAW and rawsize given, 
    // else: not raw AND explicit -S or implicit -S, i.e. no type_opts present
    if ((opts & RAW_OPT_MASK) && rawsize)
        ctx->type_maps[STR_MAP] = init_hmap(get_str_freq_key, rawsize, 
                                                          0, 0, 0, 0, 0);
    else if (!(opts & RAW_OPT_MASK) 
              && (!(opts & (TYPE_OPTS_MASK)) || (opts & STRING_OPT_MASK)))
        ctx->type_maps[STR_MAP] = init_hmap(get_str_freq_key, 0, 
                                            STRING_KEY, 0, 0, 0, 0);
    return ctx;
}

void destroy_freq_ctx(Ctx *ctx)
{
    if(ctx->buf)
        free(ctx->buf);
    ctx->buf = NULL;

    for (int i = 0; i < N_MAPPED_TYPES; i++)
        if (ctx->type_maps[i]) {
            destroy_hmap(ctx->type_maps[i]);
            ctx->type_maps[i] = NULL;
        }

    free(ctx);
}

void freq(Ctx *ctx, int opts, char *delim, int rawsize)
{
    // raw or not, ch_freqs are populated the same way
    if (opts & CHAR_OPT_MASK) {
        for (int i = 0; i < (ctx->bufsize); i++) {
            int ch_idx = (int) ctx->buf[i]; 
            ctx->ch_freqs[ch_idx].count++;
            ctx->ch_freqs[ch_idx].value = (char) ctx->buf[i];
        }
    }

    //  raw input, needs to be parsed by iterating byte by byte over input
    if ((opts & RAW_OPT_MASK) && (opts & INT_OPT_MASK)) {
        MAP(IntFreq, INT_MAP, sizeof(int), GET_TMP_INT, GET_INT, MAP_INT);
        return;
    } else if ((opts & RAW_OPT_MASK) && (opts & DOUBLE_OPT_MASK)) {
        MAP(DblFreq, DBL_MAP, sizeof(double), GET_TMP_DBL, GET_DBL, MAP_DBL);
        return;
    } else if ((opts & RAW_OPT_MASK) && rawsize) {
        MAP(StrFreq, STR_MAP, rawsize, GET_TMP_STR, GET_STR, MAP_STR);
        return;
    }

    // not raw, received text input that needs to be parsed to relevant types
    char *ln;
    int bufseek, len;

    for (bufseek = 0; /*  break condition below  */ ; bufseek += len) {
        if ((len = e_getline(ctx->buf, ctx->bufsize, &ln, bufseek)) < 0)
            break;
/*
        //char *token = get_token(regex, str, &start, &token_len);
        char  *token, *endptr;
        int   i, token_len;

        token = endptr = NULL;
        i = token_len = 0;
        while ( (token = get_token(delim, ln, &i, &token_len)) != NULL) {
            if (opts & INT_OPT_MASK) {
                int data = strntoi(token, &endptr, token_len);
                if ((endptr - token + 1) == token_len) {

                    continue;
                }
            }
            if (opts & DOUBLE_OPT_MASK) {
                double data = strntod(token, &endptr, token_len);
                if ((endptr - token + 1) == token_len) {

                    continue;
                }
            }
            if (opts & STRING_OPT_MASK || !(opts & TYPE_OPTS_MASK)) {
                
            }
            
        }
*/
    }
    return;
}

// TODO: refactor this to fit into the new function declarations
void print_freqs(Ctx *ctx, int opts)
{
    if ((opts & CHAR_OPT_MASK)) {
        ChFreq *freqs = ctx->ch_freqs;
        int n = CHAR_MAX;

        if ((opts & SORT_OPT_MASK))
            qsort_generic(freqs, n, sizeof(ChFreq), cmp_ch_freq, QSORT_DESC);
        
        int max = 0;
        for (int i = 0; i < n; i++)
            if (freqs[i].count > max)
                max = freqs[i].count;
        int w;
        for (w = 0; max > 0; max /=10, w++)
            ;

        for (int i = 0; i < n; i++) {
            if (freqs[i].count == 0)
                continue;
            
            if (isprint(freqs[i].value))
                printf("%*d %c\n", w, freqs[i].count, freqs[i].value);
            else
                printf("%*d \\x%.2X\n", w, freqs[i].count, freqs[i].value);
        }
    }

    if (opts & INT_OPT_MASK) {
        Hashmap *map = ctx->type_maps[INT_MAP];
        int n = map->n_items;

        IntFreq **freqs = (IntFreq **) emalloc(sizeof(IntFreq *) * n);
        iterate_map(map, add_int_freqs, freqs);

        if ((opts & SORT_OPT_MASK))
            qsort_generic(freqs, n, sizeof(IntFreq *), cmp_int_freq, 
                                                          QSORT_DESC);
        
        int max = 0;
        for (int i = 0; i < n; i++)
            if (freqs[i]->count > max)
                max = freqs[i]->count;
        int w;
        for (w = 0; max > 0; max /= 10, w++)
            ;

        for (int i = 0; i < n; i++) {
            printf("%*d %d\n", w, freqs[i]->count, freqs[i]->value);
            free(freqs[i]);
            freqs[i] = NULL;
        }

        free(freqs);
        freqs = NULL;
    }

    if (opts & DOUBLE_OPT_MASK) {
        Hashmap *map = ctx->type_maps[DBL_MAP];
        int n = map->n_items;

        DblFreq **freqs = (DblFreq **) emalloc(sizeof(DblFreq *) * n);
        iterate_map(map, add_dbl_freqs, freqs);

        if ((opts & SORT_OPT_MASK))
            qsort_generic(freqs, n, sizeof(DblFreq *), cmp_dbl_freq, 
                                                          QSORT_DESC);
        
        int max = 0;
        for (int i = 0; i < n; i++)
            if (freqs[i]->count > max)
                max = freqs[i]->count;
        int w;
        for (w = 0; max > 0; max /= 10, w++)
            ;

        for (int i = 0; i < n; i++) {
            printf("%*d %lf\n", w, freqs[i]->count, freqs[i]->value);
            free(freqs[i]);
            freqs[i] = NULL;
        }

        free(freqs);
        freqs = NULL;
    }

    if ((opts & STRING_OPT_MASK) || (opts & TYPE_OPTS_MASK) == 0) {
        Hashmap *map = ctx->type_maps[2];
        int n  = map->n_items;

        StrFreq **freqs = (StrFreq **) emalloc(sizeof(StrFreq *) * n);
        iterate_map(map, add_str_freqs, freqs);
        if ((opts & SORT_OPT_MASK))
            qsort_generic(freqs, n, sizeof(StrFreq *), cmp_str_freq, 
                                                          QSORT_DESC);
        
        int max = 0;
        for (int i = 0; i < n; i++)
            if (freqs[i]->count > max)
                max = freqs[i]->count;
        int w;
        for (w = 0; max > 0; max /= 10, w++)
            ;

        for (int i = 0; i < n; i++) { 
            printf("%*d '%.*s'\n", w, freqs[i]->count, freqs[i]->len, 
                                                        freqs[i]->value);
            
            free(freqs[i]->value);
            freqs[i]->value = NULL;
            free(freqs[i]);
            freqs[i] = NULL;
        }
        free(freqs);
        freqs = NULL;
    }
}

int parse_opts(int argc, char *argv[], char **delim, int *rawsize) 
{
    int opt, opts, n_rawsize_given, n_delim_given;

    opts      = 0;
    *delim    = DEFAULT_DELIM;
    *rawsize  = DEFAULT_SIZE;

    n_rawsize_given  = 0;
    n_delim_given    = 0;

    // opterr: getopt.h, 0'ing it supresses getop.h errs, see "man 3 getopt"
    opterr = 0; 
    while ((opt = getopt_long(argc, argv, OPT_STR, LongOpts, NULL)) != -1) {
        // cases 'h' ':' and '?' exit the program
        switch (opt) {              
            case 'h': e_usage(); // EXIT_SUCCESS
            case 'a': break;
            case 's': break;
            case 'D': e_set_delim(delim, &n_delim_given); break;
            case 'R': e_set_rawsize(rawsize, &n_rawsize_given); break;
            case 'c': // type options just set relevant flag bit
            case 'i':
            case 'd':
            case 'S': break;

            // optind: getopt.h, index of next arg, "man 3 getopt"
            case ':': eprintf(ErrOptReqsArg, optopt); // EXIT_FAILURE
            case '?': e_invalid_opt(argv);                
            // impossible case, EXIT_FAILURE
            default: eprintf("Unexpected option: %c:", opt);
        }
        // valid option, set the relevant flag bit
		    opts = set_opts(opts, opt);
    }
    e_validate(opts, n_rawsize_given, n_delim_given); 
    return opts;
}

/* set_opts: set the relevant bit of opts to 1 based on opt
              and the LongOpts option registry arr.
              In case of invalid options, return the original opts */
int set_opts(int opts, int opt)
{
    int i;
		for (i = 0; i < N_SUPPORTED_OPTS; i++)
				if (LongOpts[i].val == opt)
				    return ( opts | (1 << i) );
     return opts;
}

/* atoi_pos: convert a string to +integer, report if error */
int atoi_pos(char *str)
{
    char *endptr;
    long result;

    endptr = NULL;
    result = strtol(str, &endptr, 10);

    //  See man 3 strtol
    if ( !(*str && *endptr == '\0') )
        return NOT_POSITIVE_INT_ERR;

    if (result <= 0)
        return NOT_POSITIVE_INT_ERR;

    if (result < INT_MIN)
        return NOT_POSITIVE_INT_ERR;

    if (result > INT_MAX)
        return NOT_POSITIVE_INT_ERR;

    return (int) result;
}

/* e_usage: print the UsageInfoStr and exit with EXIT_SUCCESS */
void e_usage(void)
{
    weprintf(UsageInfoStr);
    exit(EXIT_SUCCESS);
}

/* e_set_delim: tries to set delim from args, exits with message if err */
void e_set_delim(char **delim, int *n_delim)
{
    *delim = estrdup(optarg);
    (*n_delim)++;
}

/* e_set_raw_size: tries to set raw size from args, exits with msg if err */
void e_set_rawsize(int *rawsize, int *n_raw)
{
    if (optarg) {
        (*n_raw)++;
        if ((*rawsize = atoi_pos(optarg)) == NOT_POSITIVE_INT_ERR)
            eprintf(ErrInvSizeArg, optarg);  // EXIT_FAILURE
    }
}

/* e_invalid_opt: in case of an invalid opt, print err msg and exit */
void e_invalid_opt(char **argv)
{
    char *inv_opt_str_tmp = NULL;
    if (optopt) 
        easprintf(&inv_opt_str_tmp, "-%c", optopt);
    else
        easprintf(&inv_opt_str_tmp, "%s", argv[optind-1]);

    eprintf(ErrInvOpt, inv_opt_str_tmp);  // EXIT_FAILURE
}

/* validate: validates the opts set by options given, 
                  if wrong usage, prints the specific err and exits */
void e_validate(int opts, int n_raw, int n_delim)
{
    int type_opts = opts & TYPE_OPTS_MASK;
    // more than one type opts with a raw option
    // binary algebraic expr results non-zero if more than 1 bit is set 
    if ((type_opts & (type_opts - 1)) && (opts & RAW_OPT_MASK))
        eprintf(ErrMultiSizeRaw);
    
    // type opt was provided alongisde an explicit raw size
    if (type_opts && n_raw)
        eprintf(ErrMultiSizeRaw);
    
    // -R was used without optional size argument, but with no type option
    if (!type_opts && (opts & RAW_OPT_MASK) && !n_raw)
        eprintf(ErrOptMutexDefault);
    
    // -R was used multiple times with size information
    if (n_raw > 1) 
        eprintf(ErrDupOptArg, 'R');
    
    // -D was used multiple times with delim information
    if (n_delim > 1)
        eprintf(ErrDupOptArg, 'D');
    
    // cannot have delims on raw binary input
    if ((opts & RAW_OPT_MASK) && (opts & DELIM_OPT_MASK))
        eprintf(ErrOptMutex, 'D', 'R');
    
    // cannot have "raw" string, we handle ascii files, it's already raw
    if ((opts & RAW_OPT_MASK) && (opts & STRING_OPT_MASK))
        eprintf(ErrOptMutex, 'S', 'R');

    return;
}

void *get_int_freq_key(Hashmap *hmap, void *data)
{
    return (void *) &(((IntFreq *)data)->value); 
}

void *get_dbl_freq_key(Hashmap *hmap, void *data)
{
    return (void *) &(((DblFreq *)data)->value);
}

void *get_str_freq_key(Hashmap *hmap, void *data)
{
    return (void *) &(((StrFreq *)data)->value);
}

int cmp_ch_freq(void *data1, void *data2)
{
    return ((ChFreq *)data1)->count - ((ChFreq *)data2)->count;
}

int cmp_int_freq(void *data1, void *data2)
{
    return ((IntFreq *)data1)->count - ((IntFreq *)data2)->count;
}

int cmp_dbl_freq(void *data1, void *data2)
{
    return ((DblFreq *)data1)->count - ((DblFreq *)data2)->count;
}

int cmp_str_freq(void *data1, void *data2)
{
    return ((StrFreq *)data1)->count - ((StrFreq *)data2)->count;
}

void add_int_freqs(Item *item, int idx, va_list args)
{
    IntFreq **freqs = va_arg(args, IntFreq **);
    freqs[idx] = (IntFreq *) item->data;
}

void add_dbl_freqs(Item *item, int idx, va_list args)
{
    DblFreq **freqs = va_arg(args, DblFreq **);
    freqs[idx] = (DblFreq *) item->data;
}

void add_str_freqs(Item *item, int idx, va_list args)
{
    StrFreq **freqs = va_arg(args, StrFreq **);
    freqs[idx] = (StrFreq *) item->data;
}

