#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "eprintf.h"
#include "hash.h"
//#include "qsort.h"

#define N_SUPPORTED_OPTS  8
#define TYPE_OPT_MASK     0xF0

enum { HELP, DELIM, RAW, SORT, INT, DOUBLE, FLOAT, LONG };

static struct option long_opts[] = {
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


void  usage       (void);
void  freq        (FILE *fin, unsigned int opt_state, char *delim);
int   parse_opts  (int argc, char **argv, unsigned int *opt_state, char **delim);


int main(int argc, char **argv)
{
    FILE *fin;
    char *delim;
    int i, next_opt_idx;
    unsigned int opt_state;

    setprogname("freq");

    opt_state = 0x0;    
    delim = " ";

    next_opt_idx = parse_opts(argc, argv, &opt_state, &delim);

    if (next_opt_idx == argc)
        freq(stdin, opt_state, delim);

    for (i = next_opt_idx; i < argc; i++)
        if ((fin = fopen(argv[i], "r")) == NULL)
            eprintf("can't open file %s", argv[i]);
        else {
            freq(fin, opt_state, delim);
            fclose(fin);
        }

    return 0;
}

void freq (FILE *fin, unsigned int opt_state, char *delim)
{
    printf("Freq! fin: %p opts: 0x%x  delim: \"%s\"\n", fin, opt_state, delim);
}

void usage(void)
{
    printf("Usage: freq [OPTIONS] file1 [file2 ...]\n"
           "Options:\n"
           "  -h, --help              Display this help and exit\n"
           "  -D, --delim=DELIM       Set delimiter (supports regex)\n"
           "  -R, --raw               Process as raw byte stream\n"
           "  -s, --sort              Sort the output\n"
           "  -i, --int               Interpret input as integers\n"
           "  -d, --double            Interpret input as doubles\n"
           "  -f, --float             Interpret input as floats\n"
           "  -l, --long              Interpret input as long integers\n"
    );
}

int parse_opts(int argc, char **argv, unsigned int *opt_state, char **delim) 
{
    int opt, opt_idx, err_flag;
    unsigned int type_opt_isolated;

    opterr = opt_idx = err_flag = 0;
    while ((opt = getopt_long(argc, argv, ":hD:Rsidfl", long_opts, &opt_idx)) != -1) {
        switch (opt) {
            case 'h':
                usage();
                exit(EXIT_SUCCESS);
            case 'D': 
                *delim = (char *) emalloc((sizeof(char) * strlen(optarg)) + 1);            
                strcpy(*delim, optarg);
                *opt_state |= 0x1 << DELIM;
                break;
            case 'R':
                *opt_state |= 0x1 << RAW;
                break;
            case 's':
                *opt_state |= 0x1 << SORT;
                break;
            case 'i':
                *opt_state |= 0x1 << INT;
                break;
            case 'd':
                *opt_state |= 0x1 << DOUBLE;
                break;
            case 'f':
                *opt_state |= 0x1 << FLOAT;
                break;
            case 'l':
                *opt_state |= 0x1 << LONG;
                break;
            case ':':
                weprintf("option -- %c requires an argument", optopt);
                err_flag = 1;
                break;
            case '?':
                weprintf("invalid option -- %c", optopt);
                err_flag = 1;
                break;
        }
    }
    if (err_flag) {
        usage();
        exit(EXIT_FAILURE);
    }

    type_opt_isolated = *opt_state & TYPE_OPT_MASK;
    if (type_opt_isolated & (type_opt_isolated - 1)) { //if more than one bit is set
        weprintf("Mutually exclusive options (-i, -d, -f, -l) cannot be used together.");
        usage();
        exit(EXIT_FAILURE);
    }

    return optind; // return the index of next opt provided by getopt.h 
}

