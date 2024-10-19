#ifndef FREQ_INTERNAL_H
#define FREQ_INTERNAL_H

#include <stdio.h> 
#include <getopt.h>

enum { HELP, AGGR, SORT, DELIM, RAW, CHAR, INT, DOUBLE, STRING };

#define N_SUPPORTED_OPTS  9 
#define OPT_STR           "+:hasD:R::cidS"

#define DEFAULT_DELIM     NULL
#define DEFAULT_SIZE      0

#define TYPE_OPTS_MASK    ((1<<INT)|(1<<CHAR)|(1<<DOUBLE))
#define RAW_OPT_MASK      ((1<<RAW))
#define DELIM_OPT_MASK    ((1<<DELIM))
#define CHAR_OPT_MASK     ((1<<CHAR))
#define STRING_OPT_MASK   ((1<<STRING))

//eatoi_positive err values
#define NOT_NUM_ERR -1
#define NEG_NUM_ERR -2

extern const char *ErrOptReqsArg;
extern const char *ErrOptMutex;
extern const char *ErrDupOptArg;
extern const char *ErrMultiSizeRaw;
extern const char *ErrInvOptChar;
extern const char *ErrInvOptStr;
extern const char *ErrInvSizeArg;
extern const char *ErrInvInteger;
extern const char *UsageInfoStr;

extern const struct option LongOpts[];

extern void freq            (FILE *fin, int flags, char *delim, int size);
extern void usage_exit      (void);
extern int  parse_opts      (int argc, char **argv, char **delim, int *size);
extern int  set_opt_flag    (int flags, int opt);
extern int  eatoi_positive  (char *str);

#endif
