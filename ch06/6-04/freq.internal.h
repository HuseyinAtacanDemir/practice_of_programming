#ifndef FREQ_INTERNAL_H
#define FREQ_INTERNAL_H

#include <stdio.h> 
#include <getopt.h>

enum { HELP, AGGR, SORT, DELIM, RAW, CHAR, INT, DOUBLE, STRING };
// parse_opts constants
#define N_SUPPORTED_OPTS  9 
#define OPT_STR           "+:hasD:R::cidS"

#define DEFAULT_DELIM     NULL
#define DEFAULT_SIZE      0

#define TYPE_OPTS_MASK    ((1<<INT)|(1<<CHAR)|(1<<DOUBLE))
#define RAW_OPT_MASK      ((1<<RAW))
#define DELIM_OPT_MASK    ((1<<DELIM))
#define CHAR_OPT_MASK     ((1<<CHAR))
#define STRING_OPT_MASK   ((1<<STRING))

// atoi_pos err values
#define NOT_POSITIVE_INT_ERR  -1

// err messsage strings
extern const char           *ErrOptReqsArg;
extern const char           *ErrOptMutex;
extern const char           *ErrOptMutexDefault;
extern const char           *ErrDupOptArg;
extern const char           *ErrMultiSizeRaw;
extern const char           *ErrInvOpt;
extern const char           *ErrInvSizeArg;

// usage message string
extern const char           *UsageInfoStr;

// getopt.h struct opton, option and argument listing, "man 3 getopt_long"
extern const struct option  LongOpts[];

extern void freq            (FILE *fin, int flags, char *delim, int size);
extern int  parse_opts      (int argc, char *argv[], char **delim, int *size);
extern int  set_opt_flag    (int flags, int opt);
extern int  atoi_pos        (char *str);
extern char *a_readline     (FILE *fin);


extern void e_usage           (void);
extern void e_set_delim       (char **delim, int *n_delim);
extern void e_set_raw_size    (int *size, int *n_raw);
extern void e_invalid_opt     (char **argv);
extern void e_validate_flags  (int flags, int n_raw, int n_delim);

#endif
