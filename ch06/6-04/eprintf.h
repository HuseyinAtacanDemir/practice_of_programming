#ifndef EPRINTF_H
#define EPRINTF_H

#include <stdio.h>

// eprintf.h: error wrapper functions
extern void   eprintf   (char *, ...);
extern void   weprintf  (char *, ...); 

extern void   efprintf  (FILE *, char *, ...);
extern void   wefprintf (FILE *, char *, ...); 

extern char   *estrdup  (char *); 
extern char   *estrndup (char *, int);

extern int    eatoi     (char *);

extern void   *emalloc  (int); 
extern void   *erealloc (void *, int); 

#endif
