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

const char *ErrOptReqsArg   = "option -%c requires an argument";
const char *ErrOptMutex     = "Option -%c cannot be used with -%c";
const char *ErrDupOptArg    = "Option -%c cannot be used more than one argument";
const char *ErrMultiSizeRaw = "Option -R cannot be used with multiple types/sizes";
const char *ErrInvOptChar   = "invalid option -%c";
const char *ErrInvOptStr    = "invalid option %s";
const char *ErrInvSizeArg   = "SIZE needs to be a positive integer. Given SIZE: %s";
const char *ErrInvInteger   = "cannot convert %s to a valid SIZE";
 
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

int parse_opts(int argc, char **argv, char **delim, int *size) 
{
    int opt, type_opts, flags, is_raw_size_provided, is_delim_provided;
    
    *delim  = DEFAULT_DELIM;
    *size   = DEFAULT_SIZE;	
    flags   = 0x0;
    is_raw_size_provided = 0;
    is_delim_provided = 0;
    
    // opterr: getopt.h, 0'ing it supresses getop.h errs, see "man 3 getopt"
    opterr = 0; 
    while ((opt = getopt_long(argc, argv, OPT_STR, LongOpts, NULL)) != -1) {
        switch (opt) {              // cases 'h' ':' and '?' exit the program
            case 'h': usage_exit(); // EXIT_SUCCESS
            case 'a': break;
            case 's': break;
            case 'D': *delim = estrdup(optarg); is_delim_provided++; break;
            case 'R': 
                if (optarg && (*size = eatoi_positive(optarg)) == NOT_NUM_ERR)
                    eprintf(ErrInvInteger, optarg);  // EXIT_FAILURE
                else if (*size == NEG_NUM_ERR)
                    eprintf(ErrInvSizeArg, optarg);   // EXIT_FAILURE
                else if (optarg)
                    is_raw_size_provided++;
                break;
            case 'c': *size = sizeof(char); break;
            case 'i': *size = sizeof(int); break;
            case 'd': *size = sizeof(double); break;
            case 'S': break;
            // optind: getopt.h, index of next arg, "man 3 getopt"
            case ':': eprintf(ErrOptReqsArg, optopt); // EXIT_FAILURE
            case '?': optopt ? eprintf(ErrInvOptChar, optopt)
                              : eprintf(ErrInvOptStr, argv[optind-1]);

            // impossible case, EXIT_FAILURE
            default: eprintf("Unexpected option: %c:", opt);
        }
		    flags = set_opt_flag(flags, opt);
    }
    
    type_opts = flags & TYPE_OPTS_MASK;
    // binary algebraic expr results non-zero if more than 1 bit is set 
    if ((type_opts & (type_opts - 1)) && (flags & RAW_OPT_MASK))
        eprintf(ErrMultiSizeRaw);
    if (type_opts && is_raw_size_provided)
        eprintf(ErrMultiSizeRaw);
    if ((flags & RAW_OPT_MASK) && (flags & DELIM_OPT_MASK))
        eprintf(ErrOptMutex, 'D', 'R'); 
    if ((flags & RAW_OPT_MASK) && (flags & STRING_OPT_MASK))
        eprintf(ErrOptMutex, 'S', 'R');
    if ((flags & CHAR_OPT_MASK) && (flags & STRING_OPT_MASK))
        eprintf(ErrOptMutex, 'S', 'c');
    if (is_raw_size_provided > 1) 
        eprintf(ErrDupOptArg, 'R');
    if (is_delim_provided > 1)
        eprintf(ErrDupOptArg, 'D');

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

/* eatoi_positive: convert a string to +integer, report if error */
int eatoi_positive(char *str)
{
    char *endptr;
    long result;

    endptr = NULL;
    result = strtol(str, &endptr, 10);

    //  See man 3 strtol
    if (!(*str && *endptr == '\0'))
        return NOT_NUM_ERR;
    if (result <= 0)
        return NEG_NUM_ERR;

    return (int) result;
}
