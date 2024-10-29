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

const char *ErrOptReqsArg       = "option -%c requires an argument";
const char *ErrOptMutex         = "Option -%c cannot be used with -%c";
const char *ErrOptMutexDefault  = "Option -R with no arguments can only be used with an explicit type option";
const char *ErrDupOptArg        = "Option -%c cannot be used more than one argument";
const char *ErrMultiSizeRaw     = "Option -R cannot be used with multiple types/sizes";
const char *ErrInvOpt           = "invalid option %s";
const char *ErrInvSizeArg       = "SIZE needs to be a positive integer. Given SIZE: %s";
 
const char *UsageInfoStr = 
"Usage: freq [-acdDhiRsS] [-D DELIM] [-R[SIZE]] [--delim=DELIM] "
                                                  "[--raw[=SIZE]] [file ...]\n"
"Options:\n"
"\t-h,        --help         Display this menu and exit\n"
"\t-a,        --aggregate    Aggregate input files into a combined output\n"
"\t-s,        --sort         Sort the output\n"
"\t-D DELIM,  --delim=DELIM  Set delimiter for non-raw input (regex)\n"
"\t-R[SIZE],  --raw[=SIZE]   Interpret input as a raw byte stream. Infers SIZE"
                                                  " from provided type option,"
                                 " or explicit SIZE argument where SIZE > 0.\n"
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

// HASH.H operates with the assumption that the user can insert whatever into 
// the map, thus hash.h needs a way of knowing what section of the bytes
// inserted corresponds to a key. To that end, when initialized, it asks for
// the key size in bytes, and a function that returns a pointer into the bytes
// written to a specific mapped item, so that using that ptr and the keysize,
// hash.h can figure out what the key is. In our case, the whole item is just
// an integer, double, or a char *, so our key_getters will be trivial
void *get_freq_key(void *data)
{
    return data; 
}

int freq_char_cmp(const void *data1, const void *data2)
{
    int *val1 = (int *) data1;
    int *val2 = (int *) data2;

    return *val1 - *val2; 
}

int freq_int_cmp(const void *data1, const void *data2)
{
    IntFreq *int_freq1 = (IntFreq *) data1;
    IntFreq *int_freq2 = (IntFreq *) data2;

    return int_freq1->count - int_freq2->count;
}

int freq_double_cmp(const void *data1, const void *data2)
{
    DoubleFreq *double_freq1 = (DoubleFreq *) data1;
    DoubleFreq *double_freq2 = (DoubleFreq *) data2;

    return double_freq1->count - double_freq2->count;
}

int freq_str_cmp(const void *data1, const void *data2)
{
    StrFreq *val1 = (StrFreq *) data1;
    StrFreq *val2 = (StrFreq *) data2;

    // we use StrFreq type for both strings, and rawdata with size parameter
    // if len's are different, StrFreq was definitely used for strings 
    if (val1->len == val2->len)
        return memcmp(val1->value, val2->value, val1->len);

    return strcmp(val1->value, val2->value); 
}

void add_int_freqs(HashmapItem *hmi, int n, va_list args)
{
    IntFreq *int_freqs = va_arg(args, IntFreq *);
    int_freqs[n].value = *((int *) hmi->data);
    int_freqs[n].count = (int)(uintptr_t)(hmi->value);
}

void add_double_freqs(HashmapItem *hmi, int n, va_list args)
{
    DoubleFreq *double_freqs = va_arg(args, DoubleFreq *);
    double_freqs[n].value = *((double *) hmi->data);
    double_freqs[n].count = (uintptr_t)(hmi->value);
}
void add_str_freqs(HashmapItem *hmi, int n, va_list args)
{
    StrFreq *str_freqs = va_arg(args, StrFreq *);
    str_freqs[n].value = (char *) hmi->data;
    str_freqs[n].count = (uintptr_t)(hmi->value);
    str_freqs[n].len   = (uintptr_t)(hmi->value);
}

Ctx *init_freq_ctx(int flags, int rawsize)
{
    Ctx *ctx = (Ctx *) emalloc(sizeof(Ctx));
    memset(ctx->freq_char, 0, sizeof(int) * UCHAR_MAX * 2);
    for (int i = 0; i < UCHAR_MAX; i++)
        ctx->freq_char[i][1] = (char) i;

    memset(ctx->type_maps, 0, sizeof(Hashmap *) * N_MAPPED_TYPES);

    if (flags & INT_OPT_MASK)
        ctx->type_maps[0] = init_hmap(get_freq_key, sizeof(int), 0, 0, 0, 0, 0);

    if (flags & DOUBLE_OPT_MASK)
        ctx->type_maps[1] = init_hmap(get_freq_key, sizeof(double), 0, 0, 0, 0, 0);

    if ((flags & RAW_OPT_MASK) && rawsize > 0)
        ctx->type_maps[3] = init_hmap(get_freq_key, rawsize, 0, 0, 0, 0, 0);

    else if ((flags & RAW_OPT_MASK) == 0 && ((flags & STRING_OPT_MASK) || (flags & TYPE_OPTS_MASK) == 0))
        ctx->type_maps[2] = init_hmap(get_freq_key, rawsize, 1, 0, 0, 0, 0);

    ctx->buf = NULL;
    ctx->bufsize = 0;
    return ctx;
}

void destroy_freq_ctx(Ctx *ctx)
{
    free(ctx->buf);
    ctx->buf = NULL;
    for (int i = 0; i < N_MAPPED_TYPES; i++)
        if (ctx->type_maps[i])
            destroy_hmap(ctx->type_maps[i]);
    free(ctx);
}

void freq(Ctx *ctx, int flags, char *delim, int rawsize)
{
    if (flags & CHAR_OPT_MASK)
        for (int i = 0; i < (ctx->bufsize); i++) {
            ctx->freq_char[ ((unsigned)ctx->buf[i]) ][0]++;
            ctx->freq_char[ ((unsigned)ctx->buf[i]) ][1] = ctx->buf[i];
        }
    
    if (flags & RAW_OPT_MASK) {
        Hashmap     *type_map = NULL;
        HashmapItem *hmi = NULL;
        int         type_size = 0;
        void        *data = NULL;
        void        *value = NULL;

        if (flags & INT_OPT_MASK) {
            type_map = ctx->type_maps[0];
            type_size = sizeof(int);
            for (int i = 0; i <= (ctx->bufsize-type_size); i += type_size) {
                int rawdata = *((int *) (ctx->buf+i));
                data = (void *) estrndup((char *) &rawdata, type_size);
                value = 0;
                hmi = find(type_map, data, CREATE, value);
                if (hmi) {
                    hmi->value++;
                }
            }
        } else if (flags & DOUBLE_OPT_MASK){
            type_map = ctx->type_maps[1];
            type_size = sizeof(double);
            for (int i = 0; i <= (ctx->bufsize-type_size); i += type_size) {
                double rawdata = *((double *) (ctx->buf+i));
                data = (void *) estrndup((char *) &rawdata, type_size);
                value = 0;
                hmi = find(type_map, data, CREATE, value);
                if (hmi) {
                    hmi->value++;
                }
            }
        } else if (rawsize){
            type_map = ctx->type_maps[2];
            type_size = rawsize;
            for (int i = 0; i <= (ctx->bufsize-type_size); i += type_size) {
                data = (char *) estrndup(ctx->buf+i, type_size);
                value = 0;
                hmi = find(type_map, data, CREATE, value);
                if (hmi) {
                    hmi->value++;
                }
            }
        }
    } else {
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

void print_freqs(Ctx *ctx, int flags)
{
    if ((flags & CHAR_OPT_MASK)) {
        int freqs = ctx->freq_char;
        int n = UCHAR_MAX;

        if ((flags & SORT_OPT_MASK))
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

    if (flags & INT_OPT_MASK) {
        Hashmap *map = ctx->type_maps[0];
        int n = map->n_elems;

        IntFreq *freqs = (IntFreq *) emalloc(sizeof(IntFreq) * n);
        iterate_hmis(map, add_int_freqs, freqs);

        if ((flags & SORT_OPT_MASK))
            qsort_generic(freqs, n, sizeof(IntFreq), freq_int_cmp, QSORT_DESC);
        
        for (int i = 0; i < n; i++) 
            printf("%d %d\n", freqs[i].count, freqs[i].value);

        free(freqs);
        freqs = NULL;
    }

    if (flags & DOUBLE_OPT_MASK) {
        Hashmap *map = ctx->type_maps[1];
        int n = map->n_elems;

        DoubleFreq *freqs = (DoubleFreq *) emalloc(sizeof(DoubleFreq) * n);
        iterate_hmis(map, add_double_freqs, freqs);

        if ((flags & SORT_OPT_MASK))
            qsort_generic(freqs, n, sizeof(DoubleFreq), freq_double_cmp, QSORT_DESC);
        
        for (int i = 0; i < n; i++) 
            printf("%d %lf\n", freqs[i].count, freqs[i].value);
        
        free(freqs);
        freqs = NULL;
    }

    if ((flags & STRING_OPT_MASK) || (flags & TYPE_OPTS_MASK) == 0) {
        Hashmap *map = ctx->type_maps[2];
        int n  = map->n_elems;

        StrFreq *freqs = (StrFreq *) emalloc(sizeof(StrFreq) * n);
        iterate_hmis(map, add_str_freqs, freqs);
        if ((flags & SORT_OPT_MASK))
            qsort_generic(freqs, n, sizeof(StrFreq), freq_str_cmp, QSORT_DESC);
        
        for (int i = 0; i < n; i++) 
            printf("%d '%s'\n", freqs[i].count, freqs[i].value);
        
        free(freqs);
        freqs = NULL;
    }
}

int parse_opts(int argc, char *argv[], char **delim, int *rawsize) 
{
    int opt, flags, n_rawsize_given, n_delim_given;

    flags     = 0;
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
		    flags = set_opt_flag(flags, opt);
    }
    e_validate_flags(flags, n_rawsize_given, n_delim_given); 
    return flags;
}

/* set_opt_flag: set the relevant bit of flags to 1 based on opt
                  and the LongOpts option registry arr.
                  In case of invalid options, return the original flags */
int set_opt_flag(int flags, int opt)
{
    int i;
		for (i = 0; i < N_SUPPORTED_OPTS; i++)
				if (LongOpts[i].val == opt)
				    return ( flags | (1 << i) );
     return flags;
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

/* validate_flags: validates the flags set by options given, 
                  if wrong usage, prints the specific err and exits */
void e_validate_flags(int flags, int n_raw, int n_delim)
{
    int type_opts = flags & TYPE_OPTS_MASK;
    // more than one type opts with a raw option
    // binary algebraic expr results non-zero if more than 1 bit is set 
    if ((type_opts & (type_opts - 1)) && (flags & RAW_OPT_MASK))
        eprintf(ErrMultiSizeRaw);
    
    // type opt was provided alongisde an explicit raw size
    if (type_opts && n_raw)
        eprintf(ErrMultiSizeRaw);
    
    // -R was used without optional size argument, but with no type option
    if (!type_opts && (flags & RAW_OPT_MASK) && !n_raw)
        eprintf(ErrOptMutexDefault);
    
    // -R was used multiple times with size information
    if (n_raw > 1) 
        eprintf(ErrDupOptArg, 'R');
    
    // -D was used multiple times with delim information
    if (n_delim > 1)
        eprintf(ErrDupOptArg, 'D');
    
    // cannot have delims on raw binary input
    if ((flags & RAW_OPT_MASK) && (flags & DELIM_OPT_MASK))
        eprintf(ErrOptMutex, 'D', 'R');
    
    // cannot have "raw" string, we handle ascii files, it's already raw
    if ((flags & RAW_OPT_MASK) && (flags & STRING_OPT_MASK))
        eprintf(ErrOptMutex, 'S', 'R');

    return;
}
