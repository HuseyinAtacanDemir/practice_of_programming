#include "regex.h"

#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "eprintf.h"

Parsed *parse_range_quantifier(char *str)
{
    char  *s = str;
    int   lower = 0;
    int   upper = 0;

    if (s == NULL)
        return NULL;

    if (*s++ != '{')
        return NULL; 

    char *endptr = NULL;
    lower = (int) strtol(s, &endptr, 10);
    if (errno == ERANGE || s == endptr || lower < 0)
        eprintf("Invalid Range Quantifier: %s", str);

    s = endptr;

    if (*s == '}')
        return get_parsed_range_quantifier(lower, upper, EXACTLY_N_TIMES);

    if (*s++ != ',')
        eprintf("Invalid Range Quantifier: %s", str);
    
    upper = (int) strtol(s, &endptr, 10);
    if (errno == ERANGE || upper < 0 || (upper && upper < lower))
        eprintf("Invalid Range Quantifier: %s", str);

    s = endptr;

    if (upper)
        return get_parsed_range_quantifier(lower, upper, N_TO_M_TIMES);
    if (*s == '}')
        return get_parsed_range_quantifier(lower, upper, N_OR_MORE_TIMES);

    eprintf("Invalid Range Quantifier: %s", str);
    return NULL;
}

Parsed *get_parsed_range_quantifier(int lower, int upper, RANGE range)
{
    RangeQuantifier *rq;
    Parsed          *pr;

    rq = (RangeQuantifier *)emalloc(sizeof(RangeQuantifier));
    rq->lower = lower;
    rq->upper = upper;
    rq->range = range;

    pr = (Parsed *)emalloc(sizeof(Parsed));
    pr->children = NULL;
    pr->result = rq;
    pr->type = RANGE_QUANTIFIER;

    return pr;
}
