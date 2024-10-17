#ifndef FREQ_INTERNAL_H
#define FREQ_INTERNAL_H

#include <stdio.h> 
#include "freq.consts.h"

extern void freq          (FILE *fin, int flags, char *delim, int size);
extern void usage_exit    (void);
extern int  parse_opts    (int argc, char **argv, char **delim, int *size);
extern int  set_opt_flag  (int flags, int opt);

#endif
