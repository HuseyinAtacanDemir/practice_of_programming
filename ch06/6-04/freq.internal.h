#ifndef FREQ_INTERNAL_H
#define FREQ_INTERNAL_H

#include <stdio.h>

enum { HELP, AGGR, DELIM, RAW, SORT, INT, DOUBLE, FLOAT, LONG, STRUCT };

extern char *USAGE_INFO_STR;

void      usage         (void);
void		  freq          (char *filename, unsigned opt_state, char *delim, int size);
unsigned  parse_opts   	(int argc, char **argv, char **delim, int *size);
void		  set_opt_bit		(unsigned *opt_state, int opt);

#endif
