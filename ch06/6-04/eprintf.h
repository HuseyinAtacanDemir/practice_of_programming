#ifndef EPRINTF_H
#define EPRINTF_H

// eprintf.h: error wrapper functions
extern void   eprintf(char *, ...);
extern void   weprintf(char *, ...); 
extern char   *estrdup(char *); 
extern void   *emalloc(int); 
extern void   *erealloc(void *, int); 

#endif
