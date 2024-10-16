#ifndef FREQ_CONSTS_H
#define FREQ_CONSTS_H

#include <getopt.h>

#define N_SUPPORTED_OPTS  10

//  The following masks are based on: - - S l  f d i s  R D a h
#define TYPE_OPTS_MASK    0x3E0   //  0 0 1 1  1 1 1 0  0 0 0 0
#define RAW_OPT_MASK      0x008   //  0 0 0 0  0 0 0 0  1 0 0 0
#define STRUCT_OPT_MASK   0x200   //  0 0 1 0  0 0 0 0  0 0 0 0

#define DEFAULT_DELIM     NULL
#define DEFAULT_SIZE      sizeof(char)
#define OPT_STR           "+:haD:RsidflS:"

enum { HELP, AGGR, DELIM, RAW, SORT, INT, DOUBLE, FLOAT, LONG, STRUCT };

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

#endif
