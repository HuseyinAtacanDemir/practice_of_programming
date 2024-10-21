#include "eprintf.h"
#include "shmalloc.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

static void vfprint_msg         (FILE *fout, const char *fmt, va_list args);
static int  evasprintf_common   (char **strp, const char *fmt,  
                                  void *(*alloc_fn)(size_t), 
                                  void (*free_fn)(void *), va_list args);

/* vfprint_msg: helper routine that prints an error/warning message with:
                1) progname if it exists, 
                2) errno info if fmt's last char is ':'*/
static void vfprint_msg(FILE *fout, const char *fmt, va_list args)
{
    fflush(fout); 
    if(getprogname() != NULL)
        fprintf(fout, "%s: ", getprogname());

    vfprintf(fout, fmt, args); 

    if (fmt[0] != '\0' && fmt[strlen(fmt)-1] == ':') 
        fprintf(fout, " %s ", strerror(errno));
    fprintf(fout, "\n");
}

/* eprintf: print error message and exit */ 
void eprintf(const char *fmt, ...)
{
    fflush(stdout); 

    va_list args;
    va_start(args, fmt); 
    vfprint_msg(stderr, fmt, args); 
    va_end(args);

    exit(EXIT_FAILURE);
}

/* weprintf: print warning message */ 
void weprintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt); 
    vfprint_msg(stderr, fmt, args); 
    va_end(args);
}

/* efprintf: print error message to file and exit */
void efprintf(FILE *fin, const char *fmt, ...)
{
    fflush(stdout); 

    va_list args;
    va_start(args, fmt); 
    vfprint_msg(fin, fmt, args); 
    va_end(args);

    exit(EXIT_FAILURE); /* conventional value forfailed execution */ 
}

/* wefprintf: print warning message to file */ 
void wefprintf(FILE *fin, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt); 
    vfprint_msg(fin, fmt, args); 
    va_end(args);
}

/* easeprintf: allocates mem and formats str that way w|e|f|printf would have 
                printed it given the same arguments, 
                returns number of chars eprintf would have printed,
                reports if error */ 
int easeprintf(char **strp, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt); 
    int n = evaseprintf(strp, fmt, args);
    va_end(args);
    
    return n;
}

int evaseprintf(char **strp, const char *fmt, va_list args)
{
    char *p = NULL;
    int n = 0;

    n = evasprintf(&p, fmt, args);
    
    // TODO: do I free the return of strerror? Should I even use strerror?
    if(getprogname() != NULL && fmt[0] != '\0' && fmt[strlen(fmt)-1] == ':')
        n = easprintf(strp, "%s: %s %s\n", getprogname(), p, strerror(errno));
    else if (getprogname() != NULL)
        n = easprintf(strp, "%s: %s\n", getprogname(), p);
    else if (fmt[0] != '\0' && fmt[strlen(fmt)-1] == ':')
        n = easprintf(strp, "%s %s\n", p, strerror(errno));
    else
        n = easprintf(strp, "%s\n", p);
    
    free(p);
    p = NULL;
    return n;
}

static int evasprintf_common(char **strp, const char *fmt,  
            void *(*alloc_fn)(size_t), void (*free_fn)(void *), va_list args)
{
    va_list args_copy;
    va_copy(args_copy, args);
    // vsnprintf with NULL and 0 calculates required size
    int size = vsnprintf(NULL, 0, fmt, args_copy);

    if (size < 0)
        return -1;

    // Allocate memory accounting for '\0'
    *strp = (char *) alloc_fn(size + 1);
    if (*strp == NULL)
        return -1;

    int result = vsnprintf(*strp, size + 1, fmt, args);

    if (result < 0) {
        free_fn(*strp);
        *strp = NULL;
        return -1;
    }

    return result;
}

/* easprintf: allocates memory and formats string, reports if err */
int easprintf(char **strp, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int result = evasprintf(strp, fmt, args);
    va_end(args);

    return result;
}

/* evasprintf: allocates memory and formats string, variadic, reports if err */
int evasprintf(char **strp, const char *fmt, va_list args)
{
    return evasprintf_common(strp, fmt, emalloc, free, args);
}

/* easprintf: allocates shared memory and formats string */
int eshasprintf(char **strp, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int result = eshvasprintf(strp, fmt, args);
    va_end(args);

    return result;
}

/* eshvasprintf: allocates shared memory and formats string, variadic */
int eshvasprintf(char **strp, const char *fmt, va_list args)
{
    return evasprintf_common(strp, fmt, eshmalloc, eshfree, args);
}

/* estrdup: duplicate a string, 
            returns pointer to the duplicate,
            report if error */ 
char *estrdup(const char *str)
{
    errno = 0;
    char *t = (char *) emalloc(strlen(str)+1); 
    strcpy(t, str);
    return t; 
}

/* estrndup:  duplicate n chars of a string, 
              returns pointer to the null terminated duplicate, 
              report if error */ 
char *estrndup(char *str, int n)
{
    errno = 0;
    char *t = (char *) emalloc(n+1); 
    strncpy(t, str, n);
    t[n] = '\0';
    return t; 
}

/* eshstrdup: duplicate a string in shared memory, 
              returns pointer to the duplicate,
              report if error */ 
char *eshstrdup(char *str)
{
    errno = 0;
    char *t = (char *) eshmalloc(strlen(str)+1); 
    strcpy(t, str);
    return t; 
}

/* eshstrndup:  duplicate n chars of a string in shared memory, 
                returns pointer to the null terminated duplicate,
                report if error */ 
char *eshstrndup(char *str, int n)
{
    errno = 0;
    char *t = (char *) eshmalloc(n+1); 
    strncpy(t, str, n);
    t[n] = '\0';
    return t; 
}


/* emalloc: malloc and report if error */ 
void *emalloc(size_t size)
{
    errno = 0;
    void *p = malloc(size); 
    if(p == NULL)
        eprintf("malloc of %u bytes failed:", size); 
    return p;
}

/* ecalloc: calloc and report if error */ 
void *ecalloc(size_t count, size_t size)
{
    errno = 0;
    void *p = calloc(count, size); 
    if(p == NULL)
        eprintf("calloc of %u bytes failed:", (count * size)); 
    return p;
}

/* erealloc: realloc and report if error */ 
void *erealloc(void *ptr, size_t size)
{
    errno = 0;
    void *q = realloc(ptr, size); 
    if(q == NULL)
        eprintf("realloc of %u bytes at %p failed:", size, ptr); 
    return q;
}

/* eshmalloc: shmalloc and report if error */
void *eshmalloc(size_t size)
{
    errno = 0;
    void *p = shmalloc(size); 
    if(p == NULL)
        eprintf("shmalloc of %u bytes failed:", size); 
    return p;

}

/* eshcalloc: shcalloc and report if error */
void *eshcalloc(size_t count, size_t size)
{
    errno = 0;
    void *q = shcalloc(count, size); 
    if(q == NULL)
        eprintf("shcalloc of %u bytes failed:", (count * size)); 
    return q;
}

/* eshrealloc: shrealloc and report if error */
void *eshrealloc(void *ptr, size_t size)
{
    errno = 0;
    void *q = shrealloc(ptr, size); 
    if(q == NULL)
        eprintf("shrealloc of %u bytes at %p failed:", size, ptr); 
    return q;
}

void eshfree(void *ptr)
{
    errno = 0;
    int result = shfree(ptr);

    if (result)
        eprintf("shfree of %p failed with err: %d:", ptr, result);
}
