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

char *MutexOpts     = "(-i -d -f -l -S)";
char *InvOptMutex   = "Mutually exclusive options %s cannot be used together";
char *InvOptChar    = "invalid option -%c";
char *InvOptStr     = "invalid option %s";
char *OptReqsArg    = "option %s requires an argument";
char *OptReqsOpt    = "option %s requires the option %s";

char *UsageInfoStr = 
"Usage: freq [-haDRsidflS] [-D DELIM] [-S size] [-i|-d|-f|-l|-S] [file ...]\n"
"Options:\n"
"\t-h      , --help         Display this help and exit\n"
"\t-a      , --aggregate    Aggregate input files into a combined output\n"
"\t-D DELIM, --delim=DELIM  Set delimiter string for non-raw input (supports regex)\n"
"\t-R      , --raw          Process input as a raw byte stream\n"
"\t-s      , --sort         Sort the output\n"
"\t-i      , --int          Interpret input as integers\n"
"\t-d      , --double       Interpret input as doubles\n"
"\t-f      , --float        Interpret input as floats\n"
"\t-l      , --long         Interpret input as long integers\n"
"\t-S SIZE , --struct=SIZE  Interpret input as packed structs of SIZE bytes, requires to -R option";

// struct option: defined in getopt.h, see man 3 getopt 
const struct option LongOpts[] = {
    {"help",      no_argument,       0, 'h'},
    {"aggregate", no_argument,       0, 'a'},
    {"delim",     required_argument, 0, 'D'},
    {"raw",       no_argument,       0, 'R'},
    {"sort",      no_argument,       0, 's'},
    {"int",       no_argument,       0, 'i'},
    {"double",    no_argument,       0, 'd'},
    {"float",     no_argument,       0, 'f'},
    {"long",      no_argument,       0, 'l'},
    {"struct",    required_argument, 0, 'S'},
    {0, 0, 0, 0}
};

void freq(char *filename, int flags, char *delim, int size)
{
		if (filename == NULL)
				return;
    return ;
}

int parse_opts(int argc, char **argv, char **delim, int *size) 
{
    int opt, type_opts, flags;
    
    *delim = DEFAULT_DELIM;
    *size = DEFAULT_SIZE;	
    flags = 0;
    
    // opterr: getopt.h global var, 0'ing it supresses getop.h errs, 
    //          see "man 3 getopt"
    opterr = 0; 
    while ((opt = getopt_long(argc, argv, OPT_STR, LongOpts, NULL)) != -1) {
        switch (opt) {
            case 'h':
								usage_exit();
            case 'D':
                *delim = estrdup(optarg);
                break;
            case 'S':
                *size = eatoi((char *) optarg);
                break;
            case 'i':
                *size = sizeof(int);
                break;
            case 'd':
                *size = sizeof(double);
                break;
            case 'f':
                *size = sizeof(float);
                break;
            case 'l':
                *size = sizeof(long);
                break;
            case ':': // optind: getopt.h, index of next arg, "man 3 getopt"
                eprintf(OptReqsArg, argv[optind-1]);
            case '?':
								if (optopt)
                		eprintf(InvOptChar, optopt);
                else 
                		eprintf(InvOptStr, argv[optind-1]);
            default: // impossible case
                assert(0);
                break; 
        }
				flags = set_opt_flag(flags, opt);
    }
    
    type_opts = flags & TYPE_OPTS_MASK;
    // binary algebraic expr results in >0 if more than 1 bit is set
    if (type_opts & (type_opts - 1))
        eprintf(InvOptMutex, MutexOpts);

    if ((flags & STRUCT_OPT_MASK) && !(flags & RAW_OPT_MASK))
        eprintf(OptReqsOpt, "-S", "-R");

    return flags;
}

/* set_opt_flag: set the relevant bit of flags to 1 based on opt
                  and the LongOpts option registry arr.
                  In case of invalid options, return the original flags */
int set_opt_flag(int flags, int opt)
{
		for (int i = 0; i < N_SUPPORTED_OPTS; i++)
				if (LongOpts[i].val == opt)
				    return ( flags | (1 << i) );
     return flags;
}

void usage_exit(void)
{
    weprintf(UsageInfoStr);
    exit(EXIT_SUCCESS);
}
