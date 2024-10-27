#include "freq.internal.h"

#include <stdlib.h>
#include <fcntl.h>
#include <getopt.h>
#include <ctype.h>

#include "eprintf.h"
#include "eread.h"
#include "qsort.h"
#include "hash.h"

int main(int argc, char **argv)
{
    setprogname("freq");

    Ctx *ctx;
    char    *delim;
    int     fd, rawsize, flags;

    flags = parse_opts(argc, argv, &delim, &rawsize);

    argc -= optind; // optind: index of next opt in argv, see "man 3 getopt"
    argv += optind;

    ctx = init_freq_ctx(flags, rawsize);

    if (argc == 0) {
        ctx->bufsize = ea_read_buf(STDIN_FILENO, &ctx->buf);
        freq(ctx, flags, delim, rawsize);
    }

    for (int i = 0; i < argc; i++) {
        if ((fd = open(argv[i], O_RDONLY)) == -1)
            eprintf("cannot open file: %s:", argv[i]);

        if (ctx == NULL)
            ctx = init_freq_ctx(flags, rawsize);

        ctx->bufsize = ea_read_buf(fd, &ctx->buf);
        freq(ctx, flags, delim, rawsize);

        if ((flags & AGGR_OPT_MASK) == 0) {
            if ((flags & CHAR_OPT_MASK)) {
                if ((flags & SORT_OPT_MASK))
                    qsort_generic(ctx->freq_char, UCHAR_MAX, sizeof(int) * 2, freq_char_cmp, QSORT_DESC);
                for (int i = 0; i < UCHAR_MAX; i++) {
                    if (isprint((char)ctx->freq_char[i][1]))
                        printf("%d %c\n", ctx->freq_char[i][0], ctx->freq_char[i][1]);
                    else
                        printf("%d \\x%.2x\n", ctx->freq_char[i][0], ctx->freq_char[i][1]);
                }
            }
            if (flags & INT_OPT_MASK) {
                int n  = ctx->type_maps[0]->n_elems;

                IntFreq *int_freqs = (IntFreq *) emalloc(sizeof(IntFreq) * n);
                iterate_hmis(ctx->type_maps[0], add_int_freqs, int_freqs);
                if ((flags & SORT_OPT_MASK))
                    qsort_generic(int_freqs, n, sizeof(IntFreq), freq_int_cmp, QSORT_DESC);
                
                for (int i = 0; i < n; i++) 
                    printf("%d, %d\n", int_freqs[i].count, int_freqs[i].value);
            }
            if (flags & DOUBLE_OPT_MASK) {
                int n  = ctx->type_maps[1]->n_elems;

                DoubleFreq *double_freqs = (DoubleFreq *) emalloc(sizeof(DoubleFreq) * n);
                iterate_hmis(ctx->type_maps[1], add_double_freqs, double_freqs);
                if ((flags & SORT_OPT_MASK))
                    qsort_generic(double_freqs, n, sizeof(DoubleFreq), freq_double_cmp, QSORT_DESC);
                
                for (int i = 0; i < n; i++) 
                    printf("%d, %lf\n", double_freqs[i].count, double_freqs[i].value);
            }
            if ((flags & STRING_OPT_MASK) || (flags & TYPE_OPTS_MASK) == 0) {
                int n  = ctx->type_maps[2]->n_elems;

                StrFreq *str_freqs = (StrFreq *) emalloc(sizeof(StrFreq) * n);
                iterate_hmis(ctx->type_maps[2], add_str_freqs, str_freqs);
                if ((flags & SORT_OPT_MASK))
                    qsort_generic(str_freqs, n, sizeof(StrFreq), freq_str_cmp, QSORT_DESC);
                
                for (int i = 0; i < n; i++) 
                    printf("%d, '%s'\n", str_freqs[i].count, str_freqs[i].value);
            }
            // if (flags & SORT_OPT_MASK) sort map
            // print freq
            destroy_freq_ctx(ctx);
            ctx = NULL;
        }

        if (close(fd))
            eprintf("cannot close file: %s:", argv[i]);
    }

    if (ctx) {
        if ((flags & CHAR_OPT_MASK)) {
            if ((flags & SORT_OPT_MASK))
                qsort_generic(ctx->freq_char, UCHAR_MAX, sizeof(int) * 2, freq_char_cmp, QSORT_DESC);
            for (int i = 0; i < UCHAR_MAX; i++)
                if (ctx->freq_char[i][0]) {
                    if (isprint((char)ctx->freq_char[i][1]))
                        printf("%d %c\n", ctx->freq_char[i][0], ctx->freq_char[i][1]);
                    else
                        printf("%d \\x%.2x\n", ctx->freq_char[i][0], (unsigned char)ctx->freq_char[i][1]);
                }
        }
        if (flags & INT_OPT_MASK) {
            int n  = ctx->type_maps[0]->n_elems;

            IntFreq *int_freqs = (IntFreq *) emalloc(sizeof(IntFreq) * n);
            iterate_hmis(ctx->type_maps[0], add_int_freqs, int_freqs);
            if ((flags & SORT_OPT_MASK))
                qsort_generic(int_freqs, n, sizeof(IntFreq), freq_int_cmp, QSORT_DESC);
            
            for (int i = 0; i < n; i++) 
                printf("%d %d\n", int_freqs[i].count, int_freqs[i].value);
        }
        if (flags & DOUBLE_OPT_MASK) {
            int n  = ctx->type_maps[1]->n_elems;

            DoubleFreq *double_freqs = (DoubleFreq *) emalloc(sizeof(DoubleFreq) * n);
            iterate_hmis(ctx->type_maps[1], add_double_freqs, double_freqs);
            if ((flags & SORT_OPT_MASK))
                qsort_generic(double_freqs, n, sizeof(DoubleFreq), freq_double_cmp, QSORT_DESC);
            
            for (int i = 0; i < n; i++) 
                printf("%d %lf\n", double_freqs[i].count, double_freqs[i].value);
        }
        if ((flags & STRING_OPT_MASK) || (flags & TYPE_OPTS_MASK) == 0) {
            int n  = ctx->type_maps[2]->n_elems;

            StrFreq *str_freqs = (StrFreq *) emalloc(sizeof(StrFreq) * n);
            iterate_hmis(ctx->type_maps[2], add_str_freqs, str_freqs);
            if ((flags & SORT_OPT_MASK))
                qsort_generic(str_freqs, n, sizeof(StrFreq), freq_str_cmp, QSORT_DESC);
            
            for (int i = 0; i < n; i++) 
                printf("%d '%s'\n", str_freqs[i].count, str_freqs[i].value);
        }
      // if (flags & SORT_OPT_MASK) sort map
      // print freq
      destroy_freq_ctx(ctx);
      ctx = NULL;
    }

   	exit(EXIT_SUCCESS);
}
