#include "freq.internal.h"

#include <fcntl.h>
#include <stdlib.h>
#include <getopt.h>
#include <limits.h>

#include "eprintf.h"
#include "eread.h"
#include "hash.h"
#include "qsort.h"

int main(int argc, char **argv)
{
    setprogname("freq");

    char      *delim, *buf, char_freq[UCHAR_MAX];
    int       fd, bufsize, rawsize, flags;
    Hashmap   *maps[3];

    flags = parse_opts(argc, argv, &delim, &rawsize);

		// optind: index of next opt in argv, see "man 3 getopt"
    argc -= optind;
    argv += optind;

    if (argc == 0) {
        bufsize = ea_read_buf(STDIN_FILENO, &buf);
        freq(buf, bufsize, flags, delim, rawsize);
    } else {
        for (int i = 0; i < argc; i++) {
            if ((fd = open(argv[i], O_RDONLY)) == -1)
                eprintf("cannot open file: %s:", argv[i]);

            bufsize = ea_read_buf(fd, &buf);
            freq(buf, bufsize, flags, delim, rawsize);

            if (close(fd))
                eprintf("cannot close file: %s:", argv[i]);
        }
    }

   	exit(EXIT_SUCCESS);
}

