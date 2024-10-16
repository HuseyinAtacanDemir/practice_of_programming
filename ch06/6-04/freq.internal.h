#ifndef FREQ_INTERNAL_H
#define FREQ_INTERNAL_H

// #include <stdio.h> //not need it now since freq takes filename not FILE *fin
#include "freq.consts.h"

extern void freq          (char *filename, int flags, char *delim, int size);
extern void usage_exit    (void);
extern int  parse_opts    (int argc, char **argv, char **delim, int *size);
extern int  set_opt_flag  (int flags, int opt);

#endif
