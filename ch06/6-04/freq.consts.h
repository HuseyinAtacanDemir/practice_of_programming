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

extern char *MutexOpts;
extern char *InvOptMutex;
extern char *InvOptChar;
extern char *InvOptStr;
extern char *OptReqsArg;
extern char *OptReqsOpt;

extern char *UsageInfoStr;

extern const struct option LongOpts[];

#endif
