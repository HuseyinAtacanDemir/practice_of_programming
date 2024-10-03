#include "eprintf.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

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

/* efprintf: print error message to file and exit */
void efprintf(FILE *fin, char *fmt, ...)
{
    va_list args;
    
    fflush(fin);
    if(getprogname() != NULL)
        fprintf(fin, "%s: ", getprogname());

    va_start(args, fmt); 
    vfprintf(fin, fmt, args); 
    va_end(args);

    if (fmt[0] != '\0' && fmt[strlen(fmt)-1] == ':') 
        fprintf(fin, " %s ", strerror(errno));
    fprintf(fin, "\n");
    exit(2); /* conventional value forfailed execution */ 
}

/* wefprintf: print warning message to file */ 
void wefprintf(FILE *fin, char *fmt, ...)
{
    va_list args;
    
    if(getprogname() != NULL)
        fprintf(fin, "%s: ", getprogname());

    va_start(args, fmt); 
    vfprintf(fin, fmt, args); 
    va_end(args);

    if (fmt[0] != '\0' && fmt[strlen(fmt)-1] == ':') 
        fprintf(fin, " %s ", strerror(errno));
    fprintf(fin, "\n");
}

/* estrdup: duplicate a string, report if error */ 
char *estrdup(char *s)
{
    char *t;
    errno = 0;
    t = (char *) malloc(strlen(s)+1); 
    if(t == NULL)
        eprintf("estrdup(\"%.20s\") failed:", s); 
    strcpy(t, s);
    return t; 
}

/* estrdup: duplicate n chars of a string, report if error */ 
char *estrndup(char *s, int n)
{
    char *t;
    errno = 0;
    t = (char *) malloc(n+1); 
    if(t == NULL)
        eprintf("estrdup(\"%.20s\") failed:", s); 
    strncpy(t, s, n);
    t[n] = '\0';
    return t; 
}

/* eatoi: convert a string to integer, report if error  */
int eatoi(char *s)
{
    char *t;
    long result;

    errno = 0;
    result = strtol(s, &t, 10);

    if (errno)
        eprintf("Error: cannot convert %s to int:", s);

    if (*t != '\0')
        eprintf("Error: invalid int format: %s", s);

    if (result > INT_MAX || result < INT_MIN)
        eprintf("Error: integer overflow converting %s", s);

    return (int) result;
}

/* emailoc: malloc and report if error */ 
void *emalloc(int n)
{
    void *p;
    errno = 0;
    p = malloc(n); 
    if(p == NULL)
        eprintf("malloc of %u bytes failed:", n); 
    return p;
}

/* erealloc: realloc and report if error */ 
void *erealloc(void *p, int  n)
{
    void *q;
    errno = 0;
    q = realloc(p, n); 
    if(q == NULL)
        eprintf("realloc of %u bytes at %p failed:", n, p); 
    return q;
}

