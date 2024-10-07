#ifndef FREQ_INTERNAL_H
#define FREQ_INTERNAL_H

#include <stdio.h>

enum { HELP, AGGR, DELIM, RAW, SORT, INT, DOUBLE, FLOAT, LONG, STRUCT };

extern char *MutexOpts;
extern char *InvOptMutex;
extern char *InvOptChar;
extern char *InvOptStr;
extern char *InvOptFormat;
extern char *OptReqsQt;
extern char *OptReqsArg;
extern char *OptSReqsArgR;
extern char *UsageInfoStr;

void      usage         (void);
void		  freq          (char *filename, unsigned opt_state, 
                          char *delim, int size);
unsigned  parse_opts   	(int argc, char **argv, char **delim, int *size);
void		  set_opt_bit		(unsigned *opt_state, int opt);

#endif
