#include "freq.test_helper.h"
#include "freq.internal.h"

#include <stdlib.h>
#include "eprintf.h"

int   get_opt_idx       (int opt);        
int   is_mutex_opts     (char opt1, char opt2);

char  **create_argv     (int argc, ...);
void  destroy_argv      (char **argv);

char  *get_eprintf_str  (const char *fmt, ...);

//char  *concat_str_arr   (char **arr, const char *delim);
//char  *rus_doll_fmt     (int n, ...);


//// region: parse_opts
//void test_parse_opts_short_combined_concat()
//{
//    int i, npass, nfail;
//    struct TestCase {
//        int       argc;
//        int       exp_size;
//        int       exp_optind; // GNU getopt intializes optind to 1
//        unsigned  exp_optstate;
//        char      *exp_msg;
//        char      **argv;
//    };
//    
//    printf("\n\tshort opts combined together\n"); 
//
//    npass = nfail = 0;
//
//    struct TestCase cases[] = {
//    // Valid combinations
//    {2, 0, 2, 0x28, NULL, 
//      create_argv(2, "./freq_test", "-iR")}, 
//    
//    {3, 0, 3, 0x0C, NULL, 
//      create_argv(3, "./freq_test", "-RD", "\"delim\"")},  
//    
//    {2, 0, 2, 0x112, NULL, 
//      create_argv(2, "./freq_test", "-sal")}, 
//    
//    {2, 0, 2, 0x88, NULL, 
//      create_argv(2, "./freq_test", "-fR")}, 
//    
//    {3, 10, 3, 0x208, NULL, 
//      create_argv(3, "./freq_test", "-RS", "10")}, 
//
//    // Invalid combinations (mutually exclusive)
//    {2, 0, 2, 0x00, rus_doll_fmt(3, "freq_test: %s\n", InvOptMutex, MutexOpts), 
//      create_argv(2, "./freq_test", "-id")}, 
//    
//    {2, 0, 2, 0x00, rus_doll_fmt(3, "freq_test: %s\n", InvOptMutex, MutexOpts), 
//      create_argv(2, "./freq_test", "-fl")}, 
//    
//    {3, 0, 3, 0x00, rus_doll_fmt(3, "freq_test: %s\n", InvOptMutex, MutexOpts), 
//      create_argv(3, "./freq_test", "-idS", "5")}, 
//   
//    // Invalid opt ordering (req arg opt should come last) 
//    {3, 0, 3, 0x00, rus_doll_fmt(3, "freq_test: %s\n", OptReqsArg, "-S"), 
//      create_argv(3, "./freq_test", "-Si", "5")}, 
//    
//    {3, 0, 3, 0x00, rus_doll_fmt(3, "freq_test: %s\n", OptReqsArg, "-S"), 
//      create_argv(3, "./freq_test","-Sd", "5")},  
//    
//    {3, 0, 3, 0x00, rus_doll_fmt(3, "freq_test: %s\n", OptReqsArg, "-S"), 
//      create_argv(3, "./freq_test","-dfiSR", "5")},  
//    
//    // Missing required arguments
//    {2, 0, 2, 0x00, rus_doll_fmt(3, "freq_test: %s\n", OptReqsQt, "-D"), 
//      create_argv(2, "./freq_test", "-DR")},  
//    
//    {2, 0, 2, 0x00, rus_doll_fmt(3, "freq_test: %s\n", OptReqsArg, "-S"), 
//      create_argv(2, "./freq_test", "-SR")},  
//
//    // Option -S requires -R
//    {3, 0, 3, 0x00, rus_doll_fmt(2, "freq_test: %s\n", OptSReqsArgR), 
//      create_argv(3, "./freq_test", "-sS", "10")},  
//    
//    {3, 10, 3, 0x208, NULL, 
//      create_argv(3, "./freq_test", "-RS", "10")},  
//
//    // Valid multiple short options
//    {3, 0, 3, 0x3E, NULL, // 0000 0011 1110 
//      create_argv(3, "./freq_test", "-iRsaD", "\",\"")},  
//    {2, 0, 2, 0x98, NULL, 
//      create_argv(2, "./freq_test", "-fRs")},
//    
//    {0, 0, 0, 0, NULL, NULL} 
//    };
//
//    for (i = 0; cases[i].argc != 0; i++) {
//         printf("\t\tcmd: \"%s\" argc: %d, exp_optstate: %u, exp_optind: %d: ", 
//                concat_str_arr(cases[i].argv, " "), cases[i].argc, 
//                cases[i].exp_optstate, cases[i].exp_optind);
//        fflush(stdout);
//        if (test_parse_opts(cases[i].argc,cases[i].argv,cases[i].exp_optstate,
//                     cases[i].exp_size, cases[i].exp_optind, cases[i].exp_msg))
//
//        if (cases[i].exp_msg)
//            free(cases[i].exp_msg);        
//        free(cases[i].argv);
//    }
//
//}
//
//void test_parse_opts_long_single()
//{
//    int i, npass, nfail;
//    struct TestCase {
//        int       argc;
//        int       exp_size;
//        int       exp_optind; // GNU getopt intializes optind to 1
//        unsigned  exp_optstate;
//        char      *exp_msg;
//        char      **argv;
//    };
//    
//    printf("\n\tlong opts single\n"); 
//
//    struct TestCase cases[] = {
//    {2, 0, 0, 0x0, rus_doll_fmt(2, "freq_test: %s\n", UsageInfoStr), 
//      create_argv(2, "./freq", "--help")},    
//
//    {2, 0, 2, 0x2, NULL, 
//      create_argv(2, "./freq", "--aggregate")},    
//
//    {2, 0, 0, 0x0, rus_doll_fmt(3, "freq_test: %s\n", OptReqsArg, "--delim"), 
//      create_argv(2, "./freq", "--delim")},    
//
//    {2, 0, 2, 0x8, NULL, 
//      create_argv(2, "./freq", "--raw")},    
//
//    {2, 0, 2, 0x10, NULL, 
//      create_argv(2, "./freq", "--sort")},    
//
//    {2, 0, 2, 0x20, NULL, 
//      create_argv(2, "./freq", "--int")},    
//
//    {2, 0, 2, 0x40, NULL, 
//      create_argv(2, "./freq", "--double")},    
//
//    {2, 0, 2, 0x80, NULL, 
//      create_argv(2, "./freq", "--float")},    
//
//    {2, 0, 2, 0x100, NULL, 
//      create_argv(2, "./freq", "--long")},    
//
//    {2, 0, 0, 0x0, rus_doll_fmt(3, "freq_test: %s\n", OptReqsArg, "--struct"), 
//      create_argv(2, "./freq", "--struct")},
//
//    {2, 0, 0, 0x0, rus_doll_fmt(3, "freq_test: %s\n", InvOptStr, "--abcd"),
//      create_argv(2, "./freq", "--abcd")}, 
//
//    {0, 0, 0, 0, NULL, NULL} 
//    };
//
//    npass = nfail = 0;
//    for (i = 0; cases[i].argc != 0; i++) {
//         printf("\t\tcmd: \"%s\" argc: %d, exp_optstate: %u, exp_optind: %d: ", 
//                concat_str_arr(cases[i].argv, " "), cases[i].argc, 
//                cases[i].exp_optstate, cases[i].exp_optind);
//        fflush(stdout);
//        if (test_parse_opts(cases[i].argc,cases[i].argv,cases[i].exp_optstate,
//                     cases[i].exp_size, cases[i].exp_optind, cases[i].exp_msg))
//
//        free(cases[i].argv);
//        if (cases[i].exp_msg)
//            free(cases[i].exp_msg);
//    }
//
//}
// 
//void test_parse_opts_long_combined()
//{
//    int i, npass, nfail;
//    struct TestCase {
//        int       argc;
//        int       exp_size;
//        int       exp_optind; // GNU getopt intializes optind to 1
//        unsigned  exp_optstate;
//        char      *exp_msg;
//        char      **argv;
//    };
//    
//    printf("\n\tlong opts combined\n"); 
//
//    struct TestCase cases[] = {
//    // Valid Cases
//    {4, 0, 4, 0x0, rus_doll_fmt(2, "freq_test: %s\n", UsageInfoStr), 
//      create_argv(4, "./freq", "--raw", "--sort", "--help")},    
//
//    {5, 0, 5, 0x36, NULL, 
//      create_argv(5, "./freq", "--aggregate", "--delim=\",\"", "--int", "--sort")},    
//
//    {6, 0, 6, 0x3E, NULL, 
//      create_argv(6, "./freq", "--int", "--sort", "--raw", "--aggregate", "--delim=\"delim\"")},    
//
//    // Invalid Mutex opts
//    {5, 0, 5, 0x0, rus_doll_fmt(3, "freq_test: %s\n", InvOptMutex, MutexOpts), 
//      create_argv(5, "./freq", "--raw", "--int", "--double", "--float")},    
//
//    {6, 0, 6, 0x0, rus_doll_fmt(3, "freq_test: %s\n", InvOptMutex, MutexOpts), 
//      create_argv(6, "./freq", "--raw", "--int", "--long", "--float", "--sort")},    
//
//    {4, 0, 4, 0x0, rus_doll_fmt(3, "freq_test: %s\n", InvOptMutex, MutexOpts), 
//      create_argv(4, "./freq", "--int", "--long", "--struct=5")},    
//
//    // Opt requires arg
//    {3, 0, 3, 0x0, rus_doll_fmt(3, "freq_test: %s\n", OptReqsArg, "--delim"), 
//      create_argv(3, "./freq", "--sort", "--delim")},    
//
//    {3, 0, 3, 0x0, rus_doll_fmt(3, "freq_test: %s\n", OptReqsArg, "--struct"), 
//      create_argv(3, "./freq", "--sort", "--struct")},   
//
//    // Opt reqs other opt 
//    {3, 0, 3, 0x0, rus_doll_fmt(2, "freq_test: %s\n", OptSReqsArgR), 
//      create_argv(3, "./freq", "--sort", "--struct=100")},   
//
//    // Opt reqs arg in quotes
//    {3, 0, 3, 0x0, rus_doll_fmt(3, "freq_test: %s\n", OptReqsQt, "-D"), 
//      create_argv(3, "./freq", "--sort", "--delim=asd")},    
//
//    // invalid arg
//    {3, 0, 3, 0x0, rus_doll_fmt(3, "freq_test: %s\n", OptReqsArg, "-S"), 
//      create_argv(3, "./freq", "--sort", "--struct=asd")},    
//
//    // no arg opt given arg
//    {3, 0, 3, 0x0, rus_doll_fmt(3, "freq_test: %s\n", InvOptStr, "-R"), 
//      create_argv(3, "./freq", "--sort", "--raw=asd")},    
//
//    // invalid opt
//    {3, 0, 3, 0x0, rus_doll_fmt(3, "freq_test: %s\n", InvOptStr, "--asdasd"), 
//      create_argv(3, "./freq", "--sort", "--asdasd")},    
//
//    {3, 0, 3, 0x0, rus_doll_fmt(3, "freq_test: %s\n", InvOptStr, "--qweqwe"), 
//      create_argv(3, "./freq", "--qweqwe", "--asdasd")},    
//
//    {0, 0, 0, 0, NULL, NULL} 
//    };
//
//    npass = nfail = 0;
//    for (i = 0; cases[i].argc != 0; i++) {
//         printf("\t\tcmd: \"%s\" argc: %d, exp_optstate: %u, exp_optind: %d: ", 
//                concat_str_arr(cases[i].argv, " "), cases[i].argc, 
//                cases[i].exp_optstate, cases[i].exp_optind);
//        fflush(stdout);
//        if (test_parse_opts(cases[i].argc,cases[i].argv,cases[i].exp_optstate,
//                     cases[i].exp_size, cases[i].exp_optind, cases[i].exp_msg))
//
//        free(cases[i].argv);
//        if (cases[i].exp_msg)
//            free(cases[i].exp_msg);
//    }
//
//}
// 
//void test_parse_opts_short_long_mixed()
//{
//    int i, npass, nfail;
//    struct TestCase {
//        int       argc;
//        int       exp_size;
//        int       exp_optind; // GNU getopt intializes optind to 1
//        unsigned  exp_optstate;
//        char      *exp_msg;
//        char      **argv;
//    };
//    
//    printf("\n\tshort-long mixed\n"); 
//
//    struct TestCase cases[] = {
//    // Valid Cases
//    {4, 0, 4, 0x0, rus_doll_fmt(2, "freq_test: %s\n", UsageInfoStr), 
//      create_argv(4, "./freq", "-R", "--sort", "--help")},    
//
//    {4, 0, 4, 0x36, NULL, 
//      create_argv(4, "./freq", "-ai", "--delim=\",\"", "--sort")},    
//
//    {3, 0, 3, 0x3E, NULL, 
//      create_argv(3, "./freq", "-isRa", "--delim=\"delim\"")},    
//
//    // Invalid Mutex opts
//    {4, 0, 4, 0x0, rus_doll_fmt(3, "freq_test: %s\n", InvOptMutex, MutexOpts), 
//      create_argv(4, "./freq", "-Ri", "-d", "--float")},    
//
//    {4, 0, 4, 0x0, rus_doll_fmt(3, "freq_test: %s\n", InvOptMutex, MutexOpts), 
//      create_argv(4, "./freq", "-Rsf", "--int", "--long")},    
//
//    {3, 0, 3, 0x0, rus_doll_fmt(3, "freq_test: %s\n", InvOptMutex, MutexOpts), 
//      create_argv(3, "./freq", "-il", "--struct=5")},    
//
//    // Opt requires arg
//    {3, 0, 3, 0x0, rus_doll_fmt(3, "freq_test: %s\n", OptReqsArg, "--delim"), 
//      create_argv(3, "./freq", "-s", "--delim")},    
//
//    {3, 0, 3, 0x0, rus_doll_fmt(3, "freq_test: %s\n", OptReqsArg, "--struct"), 
//      create_argv(3, "./freq", "-s", "--struct")},   
//
//    // Opt reqs other opt 
//    {3, 0, 3, 0x0, rus_doll_fmt(2, "freq_test: %s\n", OptSReqsArgR), 
//      create_argv(3, "./freq", "-sa", "--struct=100")},   
//
//    // Opt reqs arg in quotes
//    {3, 0, 3, 0x0, rus_doll_fmt(3, "freq_test: %s\n", OptReqsQt, "-D"), 
//      create_argv(3, "./freq", "-sf", "--delim=asd")},    
//
//    // invalid arg
//    {3, 0, 3, 0x0, rus_doll_fmt(3, "freq_test: %s\n", OptReqsArg, "-S"), 
//      create_argv(3, "./freq", "-dla", "--struct=asd")},    
//
//    // no arg opt given arg
//    {3, 0, 3, 0x0, rus_doll_fmt(3, "freq_test: %s\n", InvOptStr, "-R"), 
//      create_argv(3, "./freq", "-al", "--raw=asd")},    
//
//    // invalid opt
//    {3, 0, 3, 0x0, rus_doll_fmt(3, "freq_test: %s\n", InvOptStr, "--asdasd"), 
//      create_argv(3, "./freq", "-R", "--asdasd")},    
//
//    {3, 0, 3, 0x0, rus_doll_fmt(3, "freq_test: %s\n", InvOptStr, "-q"), 
//      create_argv(3, "./freq", "-q", "--asdasd")},    
//
//    {0, 0, 0, 0, NULL, NULL} 
//    };
//
//    npass = nfail = 0;
//    for (i = 0; cases[i].argc != 0; i++) {
//         printf("\t\tcmd: \"%s\" argc: %d, exp_optstate: %u, exp_optind: %d: ", 
//                concat_str_arr(cases[i].argv, " "), cases[i].argc, 
//                cases[i].exp_optstate, cases[i].exp_optind);
//        fflush(stdout);
//        if (test_parse_opts(cases[i].argc,cases[i].argv,cases[i].exp_optstate,
//                     cases[i].exp_size, cases[i].exp_optind, cases[i].exp_msg))
//
//        free(cases[i].argv);
//        if (cases[i].exp_msg)
//            free(cases[i].exp_msg);
//    }
//
//}
// 
//void test_parse_opts_delim()
//{
//    return;
//}
//// endregion: parse_opts
//

//
//char *concat_str_arr(char **arr, const char *delim)
//{
//    int i, delim_len, total_len;
//    char *s;
//
//    total_len = 1; // accounting for null terminator
//    delim_len = strlen(delim);
//
//    for (i = 0; arr[i] != NULL; i++)
//        total_len += strlen(arr[i]) + ((arr[i+1] != NULL) ? delim_len : 0);
//
//    s = emalloc(total_len);
//
//    // Copy strings and delimiters into `s`, strcat copies '\0' as well
//    s[0] = '\0';
//    for (i = 0; arr[i] != NULL; i++) {
//        strcat(s, arr[i]);
//        if (arr[i + 1] != NULL)
//            strcat(s, delim);
//    }
//
//    return s;
//}
//

int get_opt_idx(int opt)
{
    for (int i = 0; LongOpts[i].name != NULL; i++)
        if (LongOpts[i].val == opt)
            return i;
    return -1;
}


int is_mutex_opts(char opt1, char opt2)
{
    if (opt1 == opt2)
        return 0;
    int i, j, k;
    for (i = j = k = 0; i < N_SUPPORTED_OPTS; i++) {
        if (LongOpts[i].val == opt1 && (TYPE_OPTS_MASK & (1<<i)))
            j++;
        if (LongOpts[i].val == opt2 && (TYPE_OPTS_MASK & (1<<i)))
            k++;
    }
    return (j & k);
}

char **create_argv(int argc, ...)
{
    int i;
    char **argv;

    if (argc == 0)
        eprintf("create_argv: argc needs to be >= 1");

    argv = (char **) emalloc(sizeof(char *) * (argc+1));

    argv[0] = estrdup(getprogname());

    va_list args;
    va_start(args, argc);
    for (i = 1; i < argc; i++)
        argv[i] = estrdup(va_arg(args, char *));
    va_end(args);
   
    argv[argc] = NULL;
    return argv;
}

void destroy_argv(char **argv) {
    for (int i = 0; argv[i]; i++) {
        free(argv[i]);
        argv[i] = NULL;
    }
    free(argv);
}

char *get_eprintf_str(const char *fmt, ...)
{
    char *str = NULL;

    va_list args;
    va_start(args, fmt);
    evaseprintf(&str, fmt, args);
    va_end(args);

    return str;
}
