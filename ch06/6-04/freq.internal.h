#ifndef FREQ_INTERNAL_H
#define FREQ_INTERNAL_H

// #include <stdio.h> //not need it now since freq takes filename not FILE *fin
#include "freq.consts.h"

extern char *MutexOpts;
extern char *InvOptMutex;
extern char *InvOptChar;
extern char *InvOptStr;
extern char *OptReqsArg;
extern char *OptReqsOpt
extern char *UsageInfoStr;

extern void freq          (char *filename, int flags, char *delim, int size);
extern void usage_exit    (void);
extern int  parse_opts    (int argc, char **argv, char **delim, int *size);
extern int  set_opt_flag  (int flags, int opt);

#endif
