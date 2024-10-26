#ifndef FREQ_INTERNAL_H
#define FREQ_INTERNAL_H

#include <getopt.h>
#include <limits.h>
#include <stdarg.h>

#include "hash.h"

enum { HELP, AGGR, SORT, DELIM, RAW, CHAR, INT, DOUBLE, STRING };

// parse_opts constants
#define N_SUPPORTED_OPTS  9 
#define OPT_STR           "+:hasD:R::cidS"

#define DEFAULT_DELIM     NULL
#define DEFAULT_SIZE      0

#define N_MAPPED_TYPES    3
#define TYPE_OPTS_MASK    ((1<<INT)|(1<<CHAR)|(1<<DOUBLE))
#define AGGR_OPT_MASK     ((1<<AGGR))
#define SORT_OPT_MASK     ((1<<SORT))
#define DELIM_OPT_MASK    ((1<<DELIM))
#define RAW_OPT_MASK      ((1<<RAW))
#define CHAR_OPT_MASK     ((1<<CHAR))
#define INT_OPT_MASK      ((1<<INT))
#define DOUBLE_OPT_MASK   ((1<<DOUBLE))
#define STRING_OPT_MASK   ((1<<STRING))

// atoi_pos err values
#define NOT_POSITIVE_INT_ERR  -1

typedef struct Ctx Ctx;
struct Ctx {
    int     freq_char[UCHAR_MAX][2];
    Hashmap *type_maps[N_MAPPED_TYPES];
    char    *buf;
    int     bufsize;
};

typedef struct IntFreq IntFreq;
struct IntFreq{ int value; int count; };

typedef struct DoubleFreq DoubleFreq;
struct DoubleFreq{ double value; int count; };

typedef struct StrFreq StrFreq;
struct StrFreq{ char *value; int count; };

// err messsage strings
extern const char           *ErrOptReqsArg;
extern const char           *ErrOptMutex;
extern const char           *ErrOptMutexDefault;
extern const char           *ErrDupOptArg;
extern const char           *ErrMultiSizeRaw;
extern const char           *ErrInvOpt;
extern const char           *ErrInvSizeArg;
extern const char           *UsageInfoStr; // usage message string

// getopt.h struct opton, option and argument listing, "man 3 getopt_long"
extern const struct option  LongOpts[];

// Functions
extern Ctx  *init_freq_ctx    (int flags, int rawsize);
extern void destroy_freq_ctx  (Ctx *ctx);

extern int  freq_char_cmp     (const void *, const void *);
extern int  freq_int_cmp      (const void *, const void *);
extern int  freq_double_cmp   (const void *, const void *);
extern int  freq_str_cmp      (const void *, const void *);


extern void add_int_freqs     (HashmapItem *hmi, int n, va_list);
extern void add_double_freqs  (HashmapItem *hmi, int n, va_list);
extern void add_str_freqs     (HashmapItem *hmi, int n, va_list);

extern void freq              (Ctx *ctx, int flags, char *delim, int rawsize);

extern int  parse_opts        (int argc, char *argv[], char **delim, int *rawsize);
extern int  set_opt_flag      (int flags, int opt);
extern int  atoi_pos          (char *str);

extern void e_usage           (void);
extern void e_set_delim       (char **delim, int *n_delim);
extern void e_set_rawsize     (int *rawsize, int *n_raw);
extern void e_invalid_opt     (char **argv);
extern void e_validate_flags  (int flags, int n_raw, int n_delim);

#endif
