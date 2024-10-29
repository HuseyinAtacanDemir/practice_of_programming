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
        
        if (argc == 0 || (flags & AGGR_OPT_MASK) == 0) {
            print_freqs(ctx, flags);
            destroy_freq_ctx(ctx);
            ctx = NULL;
        }
        argv++;
    } while ((--argc) > 0);

/*
    if (argc == 0) {
        ctx = init_freq_ctx(flags, rawsize);
        ctx->bufsize = ea_read_buf(STDIN_FILENO, &ctx->buf);
        freq(ctx, flags, delim, rawsize);
        print_results(ctx, flags);
        destroy_freq_ctx(ctx);
        ctx = NULL;
        exit(EXIT_SUCCESS);
    }

    for (int i = 0; i < argc; i++) {
        if ((fd = open(argv[i], O_RDONLY)) == -1)
            eprintf("cannot open file: %s:", argv[i]);

        if (ctx == NULL)
            ctx = init_freq_ctx(flags, rawsize);

        ctx->bufsize = ea_read_buf(fd, &ctx->buf);
        freq(ctx, flags, delim, rawsize);

        if (close(fd))
            eprintf("cannot close file: %s:", argv[i]);

        if ((flags & AGGR_OPT_MASK) == 0) {
            print_results(ctx, flags);
            destroy_freq_ctx(ctx);
            ctx = NULL;
        }
    }

    if (ctx) {
      print_results(ctx, flags);
      destroy_freq_ctx(ctx);
      ctx = NULL;
    }
*/
   	exit(EXIT_SUCCESS);
}
