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
        MAP_RAW(IntFreq, INT_MAP, sizeof(int), GET_INT_VAL, MAP_INT_VAL);
        return;
    } else if ((opts & RAW_OPT_MASK) && (opts & DOUBLE_OPT_MASK)) {
        MAP_RAW(DblFreq, DBL_MAP, sizeof(double), GET_DBL_VAL, MAP_DBL_VAL);
        return;
    } else if ((opts & RAW_OPT_MASK) && rawsize) {
        MAP_RAW(StrFreq, STR_MAP, rawsize, GET_STR_VAL, MAP_STR_VAL);
        return;
    } else if ((OPTS & RAW_OPT_MASK)) {
        eprintf("impossible! how did you end up here?!"); // EXIT_FAILURE
    }

    // not raw, received text input that needs to be parsed to relevant types
    int bufseek;
    int len;
    char *ln;
    for (bufseek = 0; /*  break condition below  */ ; bufseek += len) {
        if ((len = e_getline(ctx->buf, ctx->bufsize, &ln, bufseek)) < 0)
            break;
        // if len >= 0, process for loop body: 
        int printlen = ln[len-1] == '\n' ? len-1 : len;
        printf("%.*s\n", printlen, ln);
    }

    /*
    pseudocode:
    create an dynamic array of structs with possible hmaps
    if not raw {
        for each line read {
            if -c
                iterate over line, increment relevant to char bucket
            if other type options exist alongside -c, or no options were given from the getgo {
                tokenize based on delim or whitespace
                for each token {
                    given -i && check if strtoi, check i first cause strtod will match
                        create hmap_int if not alreadyadd to hmap_int
                    else given -d, check if strtod(str, end) end-str == strlen
                        add to hmap_double
                    if none given, or explicit -S given:
                        add the whole token to hmap_str
                }
            }
        }
    } else {
        get size from a type option --if exists, or the size provided to raw as an arg
        read into a buffer until EOF, making sure buffer is multiple of size {
            create custom hmap with key size info
            populate hmap by iterating over bytes with a buffer
        }
    }


    */
    return;
}

void print_freqs(Ctx *ctx, int opts)
{
    if ((opts & CHAR_OPT_MASK)) {
        int freqs = ctx->freq_char;
        int n = UCHAR_MAX;

        if ((opts & SORT_OPT_MASK))
            qsort_generic(freqs, n, sizeof(int[2]), freq_char_cmp, QSORT_DESC);

        for (int i = 0; i < n; i++) {
            if (freqs[i][0]) {
                if (isprint((char)freqs[i][1]))
                    printf("%d %c\n", freqs[i][0], freqs[i][1]);
                else
                    printf("%d \\x%.2X\n", freqs[i][0], (char)freqs[i][1]);
            }
        }
    }

    if (opts & INT_OPT_MASK) {
        Hashmap *map = ctx->type_maps[0];
        int n = map->n_elems;

        IntFreq *freqs = (IntFreq *) emalloc(sizeof(IntFreq) * n);
        iterate_hmis(map, add_int_freqs, freqs);

        if ((opts & SORT_OPT_MASK))
            qsort_generic(freqs, n, sizeof(IntFreq), freq_int_cmp, QSORT_DESC);
        
        for (int i = 0; i < n; i++) 
            printf("%d %d\n", freqs[i].count, freqs[i].value);

        free(freqs);
        freqs = NULL;
    }

    if (opts & DOUBLE_OPT_MASK) {
        Hashmap *map = ctx->type_maps[1];
        int n = map->n_elems;

        DoubleFreq *freqs = (DoubleFreq *) emalloc(sizeof(DoubleFreq) * n);
        iterate_hmis(map, add_double_freqs, freqs);

        if ((opts & SORT_OPT_MASK))
            qsort_generic(freqs, n, sizeof(DoubleFreq), freq_double_cmp, QSORT_DESC);
        
        for (int i = 0; i < n; i++) 
            printf("%d %lf\n", freqs[i].count, freqs[i].value);
        
        free(freqs);
        freqs = NULL;
    }

    if ((opts & STRING_OPT_MASK) || (opts & TYPE_OPTS_MASK) == 0) {
        Hashmap *map = ctx->type_maps[2];
        int n  = map->n_elems;

        StrFreq *freqs = (StrFreq *) emalloc(sizeof(StrFreq) * n);
        iterate_hmis(map, add_str_freqs, freqs);
        if ((opts & SORT_OPT_MASK))
            qsort_generic(freqs, n, sizeof(StrFreq), freq_str_cmp, QSORT_DESC);
        
        for (int i = 0; i < n; i++) 
            printf("%d '%s'\n", freqs[i].count, freqs[i].value);
        
        free(freqs);
        freqs = NULL;
    }
}

int parse_opts(int argc, char *argv[], char **delim, int *rawsize) 
{
    int opt, opts, n_rawsize_given, n_delim_given;

    opts     = 0;
    *delim    = DEFAULT_DELIM;
    *rawsize  = DEFAULT_SIZE;

    n_rawsize_given  = 0;
    n_delim_given     = 0;

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
		    opts = set_opt_flag(opts, opt);
    }
    e_validate(opts, n_rawsize_given, n_delim_given); 
    return opts;
}

/* set_opt_flag: set the relevant bit of opts to 1 based on opt
                  and the LongOpts option registry arr.
                  In case of invalid options, return the original opts */
int set_opt_flag(int opts, int opt)
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

void *get_double_freq_key(Hashmap *hmap, void *data)
{
    return (void *) &(((DoubleFreq *)data)->value);
}

void *get_str_freq_key(Hashmap *hmap, void *data)
{
    return (void *) &(((StrFreq *)data)->value);
}

int char_freq_cmp(const void *data1, const void *data2)
{
    return ((CharFreq *)data1)->count - ((CharFreq *)data2)->count;
}

int int_freq_cmp(const void *data1, const void *data2)
{
    return ((IntFreq *)data1)->count - ((IntFreq *)data2)->count;
}

int double_freq_cmp(const void *data1, const void *data2)
{
    return ((DoubleFreq *)data1)->count - ((DoubleFreq *)data2)->count;
}

int str_freq_cmp(const void *data1, const void *data2)
{
    return ((StrFreq *)data1)->count - ((StrFreq *)data2)->count;
}

Ctx *init_freq_ctx(int opts, int rawsize)
{
    Ctx *ctx = (Ctx *) emalloc(sizeof(Ctx));
    
    // set everything to NULL initially
    memset(*ctx, 0, sizeof(Ctx));

    // initialize the freq arrays of corresponding types
    // since the char set is finite, we can pre allocate    
    ctx->char_freqs = (CharFreq *)emalloc(sizeof(CharFreq) * UCHAR_MAX);
    memset(ctx->char_freqs, 0, sizeof(CharFreq) * UCHAR_MAX);

    // initialize the hashmaps for mapped types only if that type is selected
    if (opts & INT_OPT_MASK)
        ctx->type_maps[0] = init_hmap(get_int_freq_key, sizeof(int), 
                                                        0, 0, 0, 0, 0);

    if (opts & DOUBLE_OPT_MASK)
        ctx->type_maps[1] = init_hmap(get_double_freq_key, sizeof(double), 
                                                              0, 0, 0, 0, 0);

    // mutex options, if RAW and given rawsize, 
    // else not raw AND explicit -S or implicit -S, i.e. no type_opts present
    if ((opts & RAW_OPT_MASK) && rawsize > 0)
        ctx->type_maps[3] = init_hmap(get_str_freq_key, rawsize, 
                                                    0, 0, 0, 0, 0);
    else if ((opts & RAW_OPT_MASK) == 0 
              && ((opts & STRING_OPT_MASK) || (opts & TYPE_OPTS_MASK) == 0))
        ctx->type_maps[2] = init_hmap(get_str_freq_key, rawsize, 
                                            STRING_KEY, 0, 0, 0, 0);

    return ctx;
}

void destroy_freq_ctx(Ctx *ctx)
{
    if(ctx->buf)
        free(ctx->buf);
    ctx->buf = NULL;
    
    if (ctx->char_freqs)
        free(ctx->char_freqs);
    ctx->char_freqs = NULL;

    if (ctx->int_freqs)
        free(ctx->int_freqs);
    ctx->int_freqs = NULL;

    if (ctx->double_freqs)
        free(ctx->double_freqs);
    ctx->double_freqs = NULL;

    if (ctx->str_freqs)
        free(ctx->str_freqs);
    ctx->str_freqs = NULL;

    if (ctx->rawsize_freqs)
        free(ctx->rawsize_freqs);
    ctx->rawsize_freqs = NULL;

    for (int i = 0; i < N_MAPPED_TYPES; i++)
        if (ctx->type_maps[i])
            destroy_hmap(ctx->type_maps[i]);

    free(ctx);
}
