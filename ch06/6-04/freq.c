#include "freq.h"

int main(int argc, char **argv)
{
    setprogname("freq");

    int 			err;
    char 			*delim;
    unsigned 	opt_state;

    opt_state = parse_opts(argc, argv, &delim);

		// optind: index of next opt in argv, see "man 3 getopt"
    if (optind == argc)
        freq(NULL, opt_state, delim);

    for ( ; optind < argc; optind++)
        freq(argv[optind], opt_state, delim);

   	exit(EXIT_SUCCESS);
}

void freq(char *filename, unsigned opt_state, char *delim)
{
		if (filename == NULL)
				return -1;
    return 0;
}

unsigned parse_opts(int argc, char **argv, char **delim) 
{
    int opt;
    unsigned type_opts, opt_state;

    // opterr: getopt.h global var, 0'ing it supresses getop.h errs, "man 3 getopt"
    opt_state = opterr = 0;	
    *delim = NULL;
    while ((opt = getopt_long(argc, argv, "+:hD:Rsidfl", LONG_OPTS, NULL)) != -1) {
        switch (opt) {
            case 'h':
								usage();
                exit(EXIT_SUCCESS);
            case 'D': 
                *delim = estrdup(optarg);
            case 'R':
            case 's':
            case 'i':
            case 'd':
            case 'f':
            case 'l':
								set_opt_bit(&opt_state, opt);
                break;
            case ':': // optind: getopt.h global var, index of next arg, "man 3 getopt"
                weprintf("option %s requires an argument", argv[optind-1]);
                exit(EXIT_FAILURE);
            case '?':
								if (optopt)		
                		weprintf("invalid option -%c", optopt);
                else
                		weprintf("invalid option %s", argv[optind-1]);
                exit(EXIT_FAILURE);
        }
    }
    
    type_opts = opt_state & TYPE_OPT_MASK;

    if (type_opts & (type_opts - 1)) { // if more than one type bit is set
        weprintf("Mutually exclusive options (%s) cannot be used together.", 
              MUTUALLY_EXCLUSIVE_OPTS);
        exit(EXIT_FAILURE);
    }

    if (*delim == NULL)
        *delim = estrdup(DEFAULT_DELIM);

    return opt_state;
}

void set_opt_bit(unsigned *opt_state, int opt)
{
		int i;
		for (i = 0; i < N_SUPPORTED_OPTS; i++)
				if (LONG_OPTS[i].val == opt)
						break;
    if (i >= N_SUPPORTED_OPTS) {
        weprintf("invalid option -%c", optopt);
        exit(EXIT_FAILURE);
    }
		*opt_state |= 1 << i;
}

void usage()
{
    weprintf(USAGE_INFO_STR);
}

