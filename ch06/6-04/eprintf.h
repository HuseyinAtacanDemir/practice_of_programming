#ifndef EPRINTF_H
#define EPRINTF_H

#include <stdio.h>
#include <stdarg.h>

// eprintf.h: error wrapper functions
extern void   eprintf     (char *, ...);
extern void   weprintf    (char *, ...); 

extern void   efprintf    (FILE *, char *, ...);
extern void   wefprintf   (FILE *, char *, ...); 

extern int    easprintf   (char **, const char *fmt, ...);
extern int    evasprintf  (char **, const char *fmt, va_list);

extern char   *estrdup    (char *); 
extern char   *estrndup   (char *, int);

extern int    eatoi       (char *);

extern void   *emalloc    (int); 
extern void   *erealloc   (void *, int); 

#endif
