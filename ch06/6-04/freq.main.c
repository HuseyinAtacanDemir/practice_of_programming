#include "freq.internal.h"

#include <fcntl.h>
#include <stdlib.h>
#include <getopt.h>

#include "eprintf.h"

int main(int argc, char **argv)
{
    setprogname("freq");

    int   fd, size, flags;
    char  *delim;

    flags = parse_opts(argc, argv, &delim, &size);

		// optind: index of next opt in argv, see "man 3 getopt"
    argc -= optind;
    argv += optind;

    if (argc == 0)
        freq(STDIN_FILENO, flags, delim, size);
    else
        for (int i = 0; i < argc; i++) {
            if ((fd = open(argv[i], O_RDONLY)) == -1)
                eprintf("cannot open file: %s:", argv[i]);

            freq(fd, flags, delim, size);

            if (close(fd))
                eprintf("cannot close file: %s:", argv[i]);
        }

   	exit(EXIT_SUCCESS);
}

