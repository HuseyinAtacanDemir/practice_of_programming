#ifndef CSV_LIB_H
#define CSV_LIB_H

// csv.h: interface for csv library

extern char   *csvgetline   (FILE *f);      // read next input line
extern char   *csvfield     (int n);        // return field n
extern int    csvnfield     (void);         // return number of fields

#endif
