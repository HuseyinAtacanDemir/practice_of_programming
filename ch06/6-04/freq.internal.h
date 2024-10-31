#ifndef FREQ_INTERNAL_H
#define FREQ_INTERNAL_H

#include <getopt.h>
#include <stdarg.h>
#include <limits.h>

#include "hash.h"

enum { HELP, AGGR, SORT, DELIM, RAW, CHAR, INT, DOUBLE, STRING };

// parse_opts constants
#define N_SUPPORTED_OPTS  9 
#define OPT_STR           "+:hasD:R::cidS"

#define DEFAULT_DELIM     NULL
#define DEFAULT_SIZE      0

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

#define N_MAPPED_TYPES  3
enum { INT_MAP, DBL_MAP, STR_MAP };

//  helper macros for parsing raw binary data
#define GET_INT(OFFSET) (*((int *)(ctx->buf+(OFFSET))))
#define GET_TMP_INT(OFFSET, _SIZE_) { 0, GET_INT((OFFSET)) }
#define MAP_INT(TYPE_STRUCT_PTR, VALUE, _SIZE_)  \
    do {  \
        TYPE_STRUCT_PTR = (IntFreq *)emalloc(sizeof(IntFreq));  \
        TYPE_STRUCT_PTR->value = (VALUE); \
        TYPE_STRUCT_PTR->count = 0; \
    } while(0)

#define GET_DBL(OFFSET) (*((double *)(ctx->buf+(OFFSET))))
#define GET_TMP_DBL(OFFSET, _SIZE_) { 0, GET_DBL((OFFSET)) }
#define MAP_DBL(TYPE_STRUCT_PTR, VALUE, _SIZE_)  \
    do {  \
        TYPE_STRUCT_PTR = (DblFreq *)emalloc(sizeof(DblFreq));  \
        TYPE_STRUCT_PTR->value = (VALUE); \
        TYPE_STRUCT_PTR->count = 0; \
    } while(0)

#define GET_STR(OFFSET) ((char *)(ctx->buf+(OFFSET)))
#define GET_TMP_STR(OFFSET, _SIZE_) { 0, GET_STR((OFFSET)), (_SIZE_) }
#define MAP_STR(TYPE_STRUCT_PTR, VALUE, _SIZE_)  \
    do {  \
        TYPE_STRUCT_PTR = (StrFreq *)emalloc(sizeof(StrFreq));  \
        TYPE_STRUCT_PTR->value  = estrndup((VALUE), (_SIZE_)); \
        TYPE_STRUCT_PTR->count  = 0; \
        TYPE_STRUCT_PTR->len    = (_SIZE_); \
    } while(0)

#define MAP(FREQ_TYPE, MAP_IDX, SIZE, GET_TMP, GET_VAL, MAP_VAL) \
    do {  \
        Hashmap *map = ctx->type_maps[(MAP_IDX)];  \
        Item *item = NULL;  \
        for (int i = 0; i <= (ctx->bufsize - (SIZE)); i += (SIZE)) {  \
            FREQ_TYPE tmp = GET_TMP(i, SIZE);  \
            if ((item = find(map, &tmp, NOT_CREATE, NULL)) != NULL) {  \
                ((FREQ_TYPE *)(item->data))->count++; \
            } else {  \
                FREQ_TYPE *inserted;  \
                MAP_VAL(inserted, GET_VAL(i), SIZE); \
                insert(map, inserted, NULL);  \
            } \
        } \
    } while (0) \

typedef struct { int count; char value; } ChFreq;

typedef struct { int count; int value; } IntFreq; 

typedef struct { int count; double value; } DblFreq;

typedef struct { int count; char *value; int len; } StrFreq;

typedef struct {
    Hashmap *type_maps[N_MAPPED_TYPES];
    ChFreq  ch_freqs[CHAR_MAX];
 
    char  *buf;
    int   bufsize;
} Ctx;

// err messsage strings
extern const char   *ErrOptReqsArg;
extern const char   *ErrOptMutex;
extern const char   *ErrOptMutexDefault;
extern const char   *ErrDupOptArg;
extern const char   *ErrMultiSizeRaw;
extern const char   *ErrInvOpt;
extern const char   *ErrInvSizeArg;
extern const char   *UsageInfoStr; // usage message string

// getopt.h struct opton, option and argument listing, "man 3 getopt_long"
extern const struct option  LongOpts[];

// Functions
extern Ctx  *init_freq_ctx    (int opts, int rawsize);
extern void destroy_freq_ctx  (Ctx *ctx);

extern void freq          (Ctx *ctx, int opts, char *delim, int rawsize);
extern void print_freqs   (Ctx *ctx, int opts);

extern int  parse_opts    (int argc, char **argv, char **delim, int *rawsize);
extern int  set_opts      (int opts, int opt);
extern int  atoi_pos      (char *str);

#endif
