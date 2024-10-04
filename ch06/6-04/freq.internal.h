#ifndef FREQ_INTERNAL_H
#define FREQ_INTERNAL_H

#include <stdio.h>

enum { HELP, DELIM, RAW, SORT, INT, DOUBLE, FLOAT, LONG, STRUCT };

void      usage         (void);
void		  freq          (char *filename, unsigned opt_state, char *delim, int size);
unsigned  parse_opts   	(int argc, char **argv, char **delim, int *size);
void		  set_opt_bit		(unsigned *opt_state, int opt);

#endif
