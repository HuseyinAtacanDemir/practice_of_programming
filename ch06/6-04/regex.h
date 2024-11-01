/*
Syntax:

*/

#ifndef REGEX_H
#define REGEX_H



typedef enum { 
    EXACTLY_N_TIMES, 
    N_OR_MORE_TIMES, 
    N_TO_M_TIMES 
} RANGE;

typedef struct {
    int   lower_bound;
    int   higher_bound;
    RANGE range;
} RangeQuantifier;

typedef enum { 
    REGEX,
    EXP,
    SUB_EXP,
    SUB_EXP_ITEM,
    MATCH,
    MATCH_ITEM,
    MATCH_CHAR_CLASS,
    MATCH_CHAR,
    GROUP,
    ANCHOR,
    BACKREF,
    QUANT,
    QUANT_TYPE,
    RANGE_QUANT,
    RANGE_QUANT_LOWER,
    RANGE_QUANT_UPPER,
    INTEGER,
    LETTER,
    CHAR_GROUP,
    CHAR_GROUP_ITEM,
    CHAR_CLASS,
    CHAR_RANGE,
    CHAR
} TYPE;


typedef struct {
    void *item;
    
} Exp;


typedef struct {
    int line_start;
    Exp **exp;
} Regex;

typedef struct {
    Parsed  **children;
    void    *result;
    TYPE    type;
} Parsed;

Parsed *parse_range_quantifier (char *str);

#endif
