#ifndef EPRINTF_H
#define EPRINTF_H

#include <stdio.h>
#include <stdarg.h>

// eprintf.h: error wrapper functions
extern void   eprintf       (char *, ...);
extern void   weprintf      (char *, ...); 

extern void   efprintf      (FILE *, char *, ...);
extern void   wefprintf     (FILE *, char *, ...); 

extern int    easprintf     (char **, const char *fmt, ...);
extern int    easeprintf    (char **, const char *fmt, ...);
extern int    evasprintf    (char **, const char *fmt, va_list);

extern int    eshasprintf   (char **, const char *fmt, ...);
extern int    eshvasprintf  (char **, const char *fmt, va_list);

extern char   *estrdup      (char *); 
extern char   *estrndup     (char *, int);

extern char   *eshstrdup    (char *); 
extern char   *eshstrndup   (char *, int);

extern int    eatoi         (char *);

extern void   *emalloc      (size_t); 
extern void   *ecalloc      (size_t, size_t);
extern void   *erealloc     (void *, size_t); 

extern void   *eshmalloc    (size_t); 
extern void   *eshcalloc    (size_t, size_t);
extern void   *eshrealloc   (void *, size_t); 
extern void   eshfree       (void *);

#endif
