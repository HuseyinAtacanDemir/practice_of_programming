#ifndef FREQ_H
#define FREQ_H

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "eprintf.h"
#include "hash.h"
#include "freq.h"
//#include "qsort.h"

#define N_SUPPORTED_OPTS  			8
#define TYPE_OPT_MASK     			0xF0
#define MUTUALLY_EXCLUSIVE_OPTS	"-i -d -f -l"
#define DEFAULT_DELIM           ""

static char *USAGE_INFO_STR = 
"Usage: freq [-hD:Rs] [-i | -d | -f | -l] file1 [file2 ...]\n"
"Options:\n"
"\t-h, --help              Display this help and exit\n"
"\t-D, --delim=DELIM       Set delimiter (supports regex)\n"
"\t-R, --raw               Process as raw byte stream\n"
"\t-s, --sort              Sort the output\n"
"\t-i, --int               Interpret input as integers\n"
"\t-d, --double            Interpret input as doubles\n"
"\t-f, --float             Interpret input as floats\n"
"\t-l, --long              Interpret input as long integers";

// struct option: defined in getopt.h, see man 3 getopt 
static struct option LONG_OPTS[] = {
    {"help",      no_argument,       0, 'h'},
    {"delim",     required_argument, 0, 'D'},
    {"raw",       no_argument,       0, 'R'},
    {"sort",      no_argument,       0, 's'},
    {"int",       no_argument,       0, 'i'},
    {"double",    no_argument,       0, 'd'},
    {"float",     no_argument,       0, 'f'},
    {"long",      no_argument,       0, 'l'},
    {0, 0, 0, 0}
};

void      usage         (void);
int		    freq          (char *filename, unsigned opt_state, char *delim);
unsigned  parse_opts   	(int argc, char **argv, char **delim);
void		  set_opt_bit		(unsigned *opt_state, int opt);

#endif
