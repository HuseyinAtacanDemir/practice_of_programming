#include "eprintf.h"
#include "shmalloc.h"

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
    exit(EXIT_FAILURE); /* conventional value forfailed execution */ 
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
    exit(EXIT_FAILURE); /* conventional value forfailed execution */ 
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

/* easprintf: allocates memory and formats string */
int easprintf(char **strp, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    va_list args_copy;
    va_copy(args_copy, args);

    // vsnprintf with NULL and 0 calculates required size
    int size = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    if (size < 0) {
        va_end(args_copy);
        return -1;
    }

    // Allocate memory accounting for '\0'
    *strp = emalloc(size + 1);
    if (*strp == NULL) {
        va_end(args_copy);
        return -1;
    }

    int result = vsnprintf(*strp, size + 1, fmt, args_copy);
    va_end(args_copy);

    if (result < 0) {
        free(*strp);
        *strp = NULL;
        return -1;
    }

    return result;
}

/* evasprintf: allocates memory and formats string, variadic */
int evasprintf(char **strp, const char *fmt, va_list args) {

    // vsnprintf with NULL and 0 calculates required size
    int size = vsnprintf(NULL, 0, fmt, args);

    if (size < 0)
        return -1;

    // Allocate memory accounting for '\0'
    *strp = emalloc(size + 1);
    if (*strp == NULL)
        return -1;

    int result = vsnprintf(*strp, size + 1, fmt, args);

    if (result < 0) {
        free(*strp);
        *strp = NULL;
        return -1;
    }

    return result;
}

/* easprintf: allocates shared memory and formats string */
int eshasprintf(char **strp, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    va_list args_copy;
    va_copy(args_copy, args);

    // vsnprintf with NULL and 0 calculates required size
    int size = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    if (size < 0) {
        va_end(args_copy);
        return -1;
    }

    // Allocate memory accounting for '\0'
    *strp = eshmalloc(size + 1);
    if (*strp == NULL) {
        va_end(args_copy);
        return -1;
    }

    int result = vsnprintf(*strp, size + 1, fmt, args_copy);
    va_end(args_copy);

    if (result < 0) {
        shfree(*strp);
        *strp = NULL;
        return -1;
    }

    return result;
}

/* eshvasprintf: allocates shared memory and formats string, variadic */
int eshvasprintf(char **strp, const char *fmt, va_list args) {

    // vsnprintf with NULL and 0 calculates required size
    int size = vsnprintf(NULL, 0, fmt, args);

    if (size < 0)
        return -1;

    // Allocate memory accounting for '\0'
    *strp = eshmalloc(size + 1);
    if (*strp == NULL)
        return -1;

    int result = vsnprintf(*strp, size + 1, fmt, args);

    if (result < 0) {
        shfree(*strp);
        *strp = NULL;
        return -1;
    }

    return result;
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

/* emalloc: malloc and report if error */ 
void *emalloc(size_t size)
{
    void *p;
    errno = 0;
    p = malloc(size); 
    if(p == NULL)
        eprintf("malloc of %u bytes failed:", size); 
    return p;
}

/* ecalloc: calloc and report if error */ 
void *ecalloc(size_t count, size_t size)
{
    void *p;
    errno = 0;
    p = calloc(count, size); 
    if(p == NULL)
        eprintf("calloc of %u bytes failed:", (count * size)); 
    return p;
}

/* erealloc: realloc and report if error */ 
void *erealloc(void *ptr, size_t size)
{
    void *q;
    errno = 0;
    q = realloc(ptr, size); 
    if(q == NULL)
        eprintf("realloc of %u bytes at %p failed:", size, ptr); 
    return q;
}

/* eshmalloc: shmalloc and report if error */
void *eshmalloc(size_t size)
{
    void *p;
    errno = 0;
    p = shmalloc(size); 
    if(p == NULL)
        eprintf("shmalloc of %u bytes failed:", size); 
    return p;

}

/* shcalloc: shcalloc and report if error */
void eshcalloc(size_t count, size_t size)
{
    void *q;
    errno = 0;
    q = shcalloc(count, size); 
    if(q == NULL)
        eprintf("shcalloc of %u bytes failed:", (count * size)); 
    return q;
}

/* eshrealloc: shrealloc and report if error */
void eshrealloc(void *ptr, size_t size)
{
    void *q;
    errno = 0;
    q = shrealloc(ptr, size); 
    if(q == NULL)
        eprintf("shrealloc of %u bytes at %p failed:", size, ptr); 
    return q;
}
