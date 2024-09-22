#ifndef CSV_H
#define CSV_H

#include <stdio.h>

#if defined(PROTO)

extern char   buf[400];
extern char   *field[20];

int   csvgetline    (FILE *fin);

#elif defined(LIB)

extern char *csvgetline (FILE *f); // read next line
extern char *csvfield   (int n); // return field n
extern int  csvnfield   (void); // return # of fields

#elif defined(LIB_4_1)

extern char *csvgetline (FILE *f); // read next line
extern char *csvfield   (int n); // return field n
extern int  csvnfield   (void); // return # of fields

#elif defined(LIB_4_2)
#elif defined(LIB_4_3)
#elif defined(LIB_4_4)

#endif

#endif
