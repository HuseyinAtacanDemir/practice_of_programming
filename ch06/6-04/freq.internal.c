#include "freq.internal.h"

#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#include "eprintf.h"
#include "hash.h"
//#include "qsort.h"

#define N_SUPPORTED_OPTS  10
                                  //  - - S l  f d i s  R D a h
#define TYPE_OPTS_MASK    0x3E0   //  0 0 1 1  1 1 1 0  0 0 0 0
#define RAW_OPT_MASK      0x008   //  0 0 0 0  0 0 0 0  1 0 0 0
#define STRUCT_OPT_MASK   0x200   //  0 0 1 0  0 0 0 0  0 0 0 0

#define DEFAULT_DELIM     ""
#define OPT_STR           "+:haD:RsidflS:"

char *MutexOpts     = "(-i -d -f -l -S)";
char *InvOptMutex   = "Mutually exclusive options %s cannot be used together";
char *InvOptChar    = "invalid option -%c";
char *InvOptStr     = "invalid option %s";
char *InvOptFormat  = "invalid option format: %s";
char *OptReqsQt     = "option %s requires an argument in quotes: \"arg\"";
char *OptReqsArg    = "option %s requires an argument";
char *OptSReqsArgR  = "option -S SIZE (--struct=SIZE) "
                      "requires -R (--raw) option.";

char *UsageInfoStr = 
"Usage: freq [-haDRsidflS] [-D DELIM] [-S size] [-i|-d|-f|-l|-S] [file ...]\n"
"Options:\n"
"\t-h      , --help         Display this help and exit\n"
"\t-a      , --aggregate    Aggregate input files into a combined output\n"
"\t-D DELIM, --delim=DELIM  Set delimiter (supports regex)\n"
"\t-R      , --raw          Process as raw byte stream\n"
"\t-s      , --sort         Sort the output\n"
"\t-i      , --int          Interpret input as integers\n"
"\t-d      , --double       Interpret input as doubles\n"
"\t-f      , --float        Interpret input as floats\n"
"\t-l      , --long         Interpret input as long integers\n"
"\t-S SIZE , --struct=SIZE  Interpret input as packed structs of SIZE bytes";

// struct option: defined in getopt.h, see man 3 getopt 
const static struct option LongOpts[] = {
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

static int is_quoted_str(char *str);

void freq(char *filename, unsigned opt_state, char *delim, int size)
{
		if (filename == NULL)
				return;
    return ;
}

unsigned parse_opts(int argc, char **argv, char **delim, int *size) 
{
    int opt;
    unsigned type_opts, opt_state;

    // opterr: getopt.h global var, 0'ing it supresses getop.h errs, 
    //          see "man 3 getopt"
    opt_state = opterr = 0;	
    *delim = NULL;
    while ((opt = getopt_long(argc, argv, OPT_STR, LongOpts, NULL)) != -1) {
        switch (opt) {
            case 'h':
								usage();
                exit(EXIT_SUCCESS);
            case 'D':
                if (!is_quoted_str(optarg)) {
                    weprintf(OptReqsQt, "-D");
                    exit(EXIT_FAILURE);
                }
                *delim = estrdup(optarg);
								set_opt_bit(&opt_state, opt);
                break;
            case 'S':
                if (!optarg || !isdigit(((char *)optarg)[0])) {
                    weprintf(OptReqsArg, "-S");
                    exit(EXIT_FAILURE);
                }
                *size = eatoi((char *) optarg);
            case 'a':
            case 'R':
            case 's':
            case 'i':
            case 'd':
            case 'f':
            case 'l':
								set_opt_bit(&opt_state, opt);
                break;
            case ':': // optind: getopt.h, index of next arg, "man 3 getopt"
                weprintf(OptReqsArg, argv[optind-1]);
                exit(EXIT_FAILURE);
            case '?':
								if (optopt) {
                		weprintf(InvOptChar, optopt);
                } else {
                		weprintf(InvOptStr, argv[optind-1]);
                }
                exit(EXIT_FAILURE);
        }
    }
    
    type_opts = opt_state & TYPE_OPTS_MASK;
    // binary algebraic expr results in >0 if more than 1 bit is set
    if (type_opts & (type_opts - 1)) { 
        weprintf(InvOptMutex, MutexOpts);
        exit(EXIT_FAILURE);
    }

    if ((opt_state & STRUCT_OPT_MASK) && !(opt_state & RAW_OPT_MASK)) {
        weprintf(OptSReqsArgR);
        exit(EXIT_FAILURE);
    }

    if (*delim == NULL)
        *delim = estrdup(DEFAULT_DELIM);

    return opt_state;
}

void set_opt_bit(unsigned *opt_state, int opt)
{
		int i;
    char buf[20];
    if (opt <= 0 || opt > UCHAR_MAX) {
        sprintf(buf, "%d", opt);
        weprintf(InvOptFormat, buf);
        exit(EXIT_FAILURE);
    }

		for (i = 0; i < N_SUPPORTED_OPTS; i++)
				if (LongOpts[i].val == opt)
						break;

    if (i >= N_SUPPORTED_OPTS) {
        weprintf(InvOptChar, opt);
        exit(EXIT_FAILURE);
    }

		*opt_state |= 1 << i;
}

void usage(void)
{
    weprintf(UsageInfoStr);
}

int is_quoted_str(char *str) 
{
    if (!str)
        return 0;
    else if (str[0] != '"')
        return 0;
    else if (strlen(str) < 2)
        return 0;
    else if (str[strlen(str)-1] != '"')
        return 0;
    return 1;
}
