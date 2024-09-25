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

#elif defined(LIB_4_1_2)
extern char *csvgetline (FILE *f); // read next line
extern char *csvfield   (int n); // return field n
extern int  csvnfield   (void); // return # of fields
#elif defined(LIB_4_3)
extern int  init        (int n_line, int n_field); // initialize
extern char *csvgetline (FILE *f); // read next line
extern char *csvfield   (int n); // return field n
extern int  csvnfield   (void); // return # of fields
extern void destroy     (void); // free up resources
#elif defined(LIB_4_4)
extern char *csvformat(char **data, int n_field, char *fmt)
#elif defined(LIB_4_8)
typedef struct CSV CSV;

extern CSV  *csvnew     (FILE *f, int n_line, int n_field, char *field_sep);
extern char *csvgetline (CSV *csv);
extern char *csvfield   (CSV *csv, int n);
extern int  csvnfield   (CSV *csv);  
#endif

#endif
