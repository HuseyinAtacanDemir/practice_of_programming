#include "eprintf.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

/* eprintf: print error message and exit */ 
void eprintf(char *fmt, ...)
{
    va_list args;

    fflush(stdout); 
    if(getprogname() != NULL)
        fprintf(stderr, "%s: ", getprogname());

    va_start(args, fmt); 
    vfprintf(stderr, fmt, args); 
    va_end(args);

    if (fmt[0] != '\0' && fmt[strlen(fmt)-1] == ':') 
        fprintf(stderr, " %s ", strerror(errno));
    fprintf(stderr, "\n");
    exit(2); /* conventional value forfailed execution */ 
}

/* weprintf: print warning message */ 
void weprintf(char *fmt, ...)
{
    va_list args;
 
    if(getprogname() != NULL)
        fprintf(stderr, "%s: ", getprogname());

    va_start(args, fmt); 
    vfprintf(stderr, fmt, args); 
    va_end(args);

    if (fmt[0] != '\0' && fmt[strlen(fmt)-1] == ':') 
        fprintf(stderr, " %s ", strerror(errno));
    fprintf(stderr, "\n");
}

/* estrdup: duplicate a string, report if error */ 
char *estrdup(char *s)
{
    char *t;
    t = (char *) malloc(strlen(s)+1); 
    if(t == NULL)
        eprintf("estrdup(\"%.20s\") failed:",s); 
    strcpy(t, s);
    return t; 
}

/* emailoc: malloc and report if error */ 
void *emalloc(int n)
{
    void *p;
    p = malloc(n); 
    if(p == NULL)
        eprintf("malloc of %u bytes failed:", n); 
    return p;
}

/* erealloc: realloc and report if error */ 
void *erealloc(void *p, int  n)
{
    void *q;
    q = realloc(p, n); 
    if(q == NULL)
        eprintf("realloc of %u bytes at %p failed:", n, p); 
    return q;
}

