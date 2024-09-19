#ifndef CSV_H
#define CSV_H

#include <stdio.h>
extern char   buf[200];
extern char   *field[20];

int   csvgetline    (FILE *fin);

#endif
