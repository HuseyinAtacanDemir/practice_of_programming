#include "freq.internal.h"

int main(int argc, char **argv)
{
    setprogname("freq");

    int 			struct_nbytes;
    char 			*delim;
    unsigned 	opt_state;

    opt_state = parse_opts(argc, argv, &delim, &struct_nbytes);

		// optind: index of next opt in argv, see "man 3 getopt"
    if (optind == argc)
        freq(NULL, opt_state, delim, struct_nbytes);

    for ( ; optind < argc; optind++)
        freq(argv[optind], opt_state, delim, struct_nbytes);

   	exit(EXIT_SUCCESS);
}

