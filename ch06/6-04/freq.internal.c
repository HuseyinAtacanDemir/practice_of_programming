#include "freq.internal.h"

#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <assert.h>

#include "eprintf.h"
#include "hash.h"
//#include "qsort.h"

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

// struct option: defined in getopt.h, see man 3 getopt 
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
    return ;
}

int parse_opts(int argc, char *argv[], char **delim, int *size) 
{
    int opt, flags, n_raw_size_given, n_delim_given;
    char *inv_opt_str = NULL;

    flags = n_raw_size_given = n_delim_given = 0;

    // opterr: getopt.h, 0'ing it supresses getop.h errs, see "man 3 getopt"
    opterr = 0; 
    while ((opt = getopt_long(argc, argv, OPT_STR, LongOpts, NULL)) != -1) {
        // cases 'h' ':' and '?' exit the program
        switch (opt) {              
            case 'h': usage_exit(); // EXIT_SUCCESS
            case 'a': break;
            case 's': break;
            case 'D': *delim = estrdup(optarg); n_delim_given++; break;
            case 'R': 
                if (optarg) {
                    n_raw_size_given++;
                    if ((*size = atoi_pos(optarg)) == NOT_POSITIVE_INT_ERR)
                        eprintf(ErrInvSizeArg, optarg);  // EXIT_FAILURE
                }
                break;
            case 'c': // type options just set relevant flag bit
            case 'i':
            case 'd':
            case 'S': break;
            // optind: getopt.h, index of next arg, "man 3 getopt"
            case ':': eprintf(ErrOptReqsArg, optopt); // EXIT_FAILURE
            case '?':
                if (optopt) 
                    easprintf(&inv_opt_str, "-%c", optopt);
                else
                    easprintf(&inv_opt_str, "%s", argv[optind-1]);
                eprintf(ErrInvOpt, inv_opt_str);  // EXIT_FAILURE
                
            // impossible case, EXIT_FAILURE
            default: eprintf("Unexpected option: %c:", opt);
        }
		    flags = set_opt_flag(flags, opt);
    }
    
    // TODO: validation in a different function?
    int type_opts = flags & TYPE_OPTS_MASK;
    // binary algebraic expr results non-zero if more than 1 bit is set 
    if ((type_opts & (type_opts - 1)) && (flags & RAW_OPT_MASK))
        eprintf(ErrMultiSizeRaw);
    
    // -R was used multiple times with size information
    if (n_raw_size_given > 1) 
        eprintf(ErrDupOptArg, 'R');
    
    // -R was used without optional size argument, but with no type option
    if (n_raw_size_given == 0 && (flags == RAW_OPT_MASK))
        eprintf(ErrOptMutexDefault);
    
    // -D was used multiple times with delim information
    if (n_delim_given > 1)
        eprintf(ErrDupOptArg, 'D');
    
    // type opt was provided alongisde a raw size
    if (type_opts && n_raw_size_given)
        eprintf(ErrMultiSizeRaw);
    
    // cannot have delims on raw binary input
    if ((flags & RAW_OPT_MASK) && (flags & DELIM_OPT_MASK))
        eprintf(ErrOptMutex, 'D', 'R');
    
    // cannot have "raw" string, we handle ascii files, it's already raw
    if ((flags & RAW_OPT_MASK) && (flags & STRING_OPT_MASK))
        eprintf(ErrOptMutex, 'S', 'R');

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

void usage_exit(void)
{
    weprintf(UsageInfoStr);
    exit(EXIT_SUCCESS);
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

