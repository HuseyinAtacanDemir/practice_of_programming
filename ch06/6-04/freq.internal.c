#include "freq.internal.h"

#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#include "eprintf.h"
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
"Usage: freq [-acdDhiRsS] [-D DELIM] [-R[SIZE]] [--delim=DELIM] [--raw[=SIZE]] [file ...]\n"
"Options:\n"
"\t-h,        --help         Display this menu and exit\n"
"\t-a,        --aggregate    Aggregate input files into a combined output\n"
"\t-s,        --sort         Sort the output\n"
"\t-D DELIM,  --delim=DELIM  Set delimiter string for non-raw input (supports regex)\n"
"\t-R[SIZE],  --raw[=SIZE]   Interpret input as a raw byte stream. Infers SIZE"
" from provided type option, or explicit SIZE argument where SIZE > 0.\n"
"\t-c,        --char         Interpret input as ASCII chars\n"
"\t-i,        --int          Interpret input as integers\n"
"\t-d,        --double       Interpret input as doubles\n"
"\t-S,        --string       Interpret input as strings";

// struct option: defined in getopt.h, see "man 3 getopt_long"
const struct option LongOpts[] = {
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

void freq(FILE *fin, int flags, char *delim, int size)
{
		if (fin == NULL)
				return;
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

    if not aggregate or (aggregate and last file) {
        if -c {
            just print whole char buckets
        }
        if sort {
            for all maps
                create array of len n_elems of map
                sort the array

            print the sorted arrays in proper order:
                in the order of option declaration
        } else {
            print regular traversal of maps in the order of option declaration
        }
        free all resources and nullify
    }

    */
    return;
}

int parse_opts(int argc, char *argv[], char **delim, int *size) 
{
    int opt, flags, n_raw_size_given, n_delim_given;

    flags   = 0;
    *delim  = DEFAULT_DELIM;
    *size   = DEFAULT_SIZE;

    n_raw_size_given  = 0;
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
            case 'R': e_set_raw_size(size, &n_raw_size_given); break;
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
		    flags = set_opt_flag(flags, opt);
    }
    e_validate_flags(flags, n_raw_size_given, n_delim_given); 
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
    if (!(*str && *endptr == '\0') || result <= 0 
      || (int) result <= 0 || result > INT_MAX)
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
void e_set_raw_size(int *size, int *n_raw)
{
    if (optarg) {
        (*n_raw)++;
        if ((*size = atoi_pos(optarg)) == NOT_POSITIVE_INT_ERR)
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
