#include "freq.internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "eprintf.h"

int main(int argc, char **argv)
{
    setprogname("freq");

    FILE  *fin;
    char  *delim;
    int 	size;
    int   flags;

    delim = NULL;
    flags = parse_opts(argc, argv, &delim, &size);

    printf("delim: %s, size: %d, flags: %x\n", 
            (delim ? delim : "NULL"), size, flags);

		// optind: index of next opt in argv, see "man 3 getopt"
    argc -= optind;
    argv += optind;

    if (argc == 0)
        freq(stdin, flags, delim, size);
    else
        for (int i = 0; i < argc; i++) {
            if ((fin = fopen(argv[i], "r")) == NULL)
                eprintf("cannot open file: %s:", argv[i]);

            freq(fin, flags, delim, size);

            if (fclose(fin) == EOF)
                eprintf("cannot close file: %s:", argv[i]);
        }

   	exit(EXIT_SUCCESS);
}

