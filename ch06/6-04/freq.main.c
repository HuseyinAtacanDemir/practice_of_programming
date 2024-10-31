#include "freq.internal.h"

#include <stdlib.h>
#include <fcntl.h>
#include <getopt.h>
#include <ctype.h>

#include "eprintf.h"
#include "eread.h"

int main(int argc, char **argv)
{
    setprogname("freq");

    Ctx   *ctx    = NULL;
    char  *delim  = NULL;
    int   fd, rawsize, flags;

    flags = parse_opts(argc, argv, &delim, &rawsize);

    argc -= optind; // optind: index of next opt in argv, see "man 3 getopt"
    argv += optind;

    do {
        if (ctx == NULL)
            ctx = init_freq_ctx(flags, rawsize);

        if (argc == 0)
            fd = STDIN_FILENO;
        else if ((fd = open(*argv, O_RDONLY)) == -1)
            eprintf("cannot open file: %s:", *argv);
            
        ctx->bufsize = ea_read_buf(fd, &ctx->buf);
        freq(ctx, flags, delim, rawsize);

        if (fd != STDIN_FILENO && close(fd))
            eprintf("cannot close file: %s:", *argv);
        
        if (argc == 0 || !(flags & AGGR_OPT_MASK)) {
            print_freqs(ctx, flags);
            destroy_freq_ctx(ctx);
            ctx = NULL;
        }
        argv++;
    } while ((--argc) > 0);

   	exit(EXIT_SUCCESS);
}
