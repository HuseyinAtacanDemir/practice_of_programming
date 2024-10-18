#include "freq.internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#include "jankunit.h"
#include "eprintf.h"

// HELPERS
  int   get_opt_idx       (int opt);        
  char  **create_argv     (int argc, ...);
  int   is_mutex_opts     (char opt1, char opt2);
  //char  *concat_str_arr   (char **arr, const char *delim);
  //char  *rus_doll_fmt     (int n, ...);

int main(void)
{
    init_ctx();
    TEST_PROGRAM("FREQ INTERNAL UNIT TESTS") {
        TEST_SUITE("SUITE: set_opt_flag unit tests") {
            TEST("test all chars") {
                int i, opt_idx;
                for (i = 0; i < (UCHAR_MAX+1); i++) {
                    opt_idx = get_opt_idx(i);
                    if (opt_idx >= 0)
                        EXPECT_EQ((1<<opt_idx), set_opt_flag(0x0, i));
                    else
                        EXPECT_EQ(0x0, set_opt_flag(0x0, i));
                }
            }
            TEST("test boundaries") {
                int cases[] = { INT_MAX, INT_MIN, UCHAR_MAX+1, CHAR_MIN-1, 0 };
                for (int i = 0; cases[i] != 0; i++)
                    EXPECT_EQ(0x0, set_opt_flag(0x0, cases[i]));
            }
            TEST("test non zero initial flags") {
                int i, opt_idx, flags;
                for (i = flags = 0; i < (UCHAR_MAX+1); i++) {
                    opt_idx = get_opt_idx(i);
                    if (opt_idx >= 0)
                        EXPECT_EQ((flags |= (1<<opt_idx)), set_opt_flag(flags, i));
                    else
                        EXPECT_EQ(flags, set_opt_flag(flags, i));
                }
            }
        }
        TEST_SUITE("SUITE: parse_opts unit tests") {
            TEST("test boundary no args") {
                FORK() {
                    int   argc = 1;
                    char  **argv = create_argv(argc, "./freq_test");

                    char  *delim = NULL;
                    int   size = 0;

                    int result = parse_opts(argc, argv, &delim, &size);

                    EXPECT_EQ_PTR(delim, DEFAULT_DELIM);
                    EXPECT_EQ(size, DEFAULT_SIZE);
                    EXPECT_EQ(result, 0x0);
                    EXPECT_EQ(optind, argc);
                } 
                EXPECT_EXIT_CODE_EQ(EXIT_SUCCESS);
                EXPECT_SIGNAL_CODE_EQ(NOT_SIGNALED);
                EXPECT_OUT_EQ("");
                EXPECT_ERR_EQ("");
            }
            TEST("test single short invalid opts") {
                for (int i = 0; i < UCHAR_MAX + 1; i++) {
                    int opt_idx = get_opt_idx(i);
                    // '\0' transposes option to just -
                    // '-'  creates the "--" opt, which has a special meaning
                    //      int getopt, so it is valid 
                    // see "man 3 getopt" or "man 3 getopt_long"
                    if (opt_idx >= 0 || i == '\0' || i == '-')
                        continue;
                    FORK() {
                        char opt[3] = { '-', i, '\0' };
                        
                        int argc    = 2;
                        char **argv = create_argv(argc, "./freq_test", opt);
                        char *delim = DEFAULT_DELIM;
                        int size    = DEFAULT_SIZE;
                        
                        parse_opts(argc, argv, &delim, &size);
                        //should not reach here, we are testing inv opts only
                        ASSERT_TRUE(0);
                    }

                    // should not have printed to stdout
                    // should not have gotten any signals
                    EXPECT_OUT_EQ("");
                    EXPECT_SIGNAL_CODE_EQ(NOT_SIGNALED);
                    
                    char *exp_err = NULL;
                    if (i == '%')
                        easeprintf(&exp_err, InvOptStr, "-%%");
                    else
                        easeprintf(&exp_err, InvOptChar, i);
                    
                    EXPECT_ERR_EQ((exp_err ? exp_err : ""));
                    EXPECT_EXIT_CODE_EQ(EXIT_FAILURE);

                    free(exp_err);
                    exp_err = NULL;
                }
            }
            TEST("test single short valid opts") {
                for (int i = 0; i < UCHAR_MAX + 1; i++) {
                    int opt_idx = get_opt_idx(i);
                    // filter out invalid options
                    if (opt_idx < 0 & i != '\0' & i != '-')
                        continue;
                    
                    FORK() {
                        char opt[3] = { '-', i, '\0' };
                        
                        int argc    = 2;
                        char **argv = create_argv(argc, "./freq_test", opt);
                        char *delim = DEFAULT_DELIM;
                        int size    = DEFAULT_SIZE;
                        
                        int flags = parse_opts(argc, argv, &delim, &size);

                        // getopt.h: int optind, see "man 3 getopt"
                        EXPECT_EQ(optind, ((i != '\0') ? argc : argc-1));
                        EXPECT_EQ_PTR(delim, DEFAULT_DELIM);

                        if (opt_idx == INT)
                            EXPECT_EQ(size, sizeof(int));
                        else if (opt_idx == DOUBLE) 
                            EXPECT_EQ(size, sizeof(double));
                        else if (opt_idx == FLOAT)
                            EXPECT_EQ(size, sizeof(float));
                        else if (opt_idx == LONG)
                            EXPECT_EQ(size, sizeof(long));
                        else
                            EXPECT_EQ(size, DEFAULT_SIZE);
                        
                        if (i != '\0' && i != '-')
                            EXPECT_EQ(flags, (1<<opt_idx));
                        else
                            EXPECT_EQ(flags, 0x0);                        
                    }

                    EXPECT_OUT_EQ("");
                    EXPECT_SIGNAL_CODE_EQ(NOT_SIGNALED);

                    char  *exp_err = NULL;
                    int   exp_exit = EXIT_SUCCESS;
                    if (i == 'h')
                        easeprintf(&exp_err, UsageInfoStr);
                    else if (LongOpts[opt_idx].has_arg == required_argument) {
                        easeprintf(&exp_err, OptReqsArg, i);
                        exp_exit = EXIT_FAILURE;
                    }
                    
                    EXPECT_ERR_EQ((exp_err ? exp_err : ""));
                    EXPECT_EXIT_CODE_EQ(exp_exit);

                    if (exp_err) {
                        free(exp_err);
                        exp_err = NULL;
                    }
                }
            }
            TEST("test separately combined 2 valid short opts") {
                for (int i = 0; i < N_SUPPORTED_OPTS-1; i++) {
                    for (int j = i+1; j < N_SUPPORTED_OPTS; j++)
                        for (int k = 0; k < 2; k++) {
                            char opts[2][3] = { 
                                {'-', LongOpts[(k%2 ? j : i)].val, '\0'}, 
                                {'-', LongOpts[(k%2 ? i : j)].val, '\0'}
                            };
                            int argc = 3;
                            FORK() {
                                int   flags  = 0x0;
                                int   size   = DEFAULT_SIZE;
                                char  *delim = DEFAULT_DELIM;
                                char  **argv = create_argv(argc, "./freq_test", 
                                                            opts[0], opts[1]);

                                flags = parse_opts(argc, argv, &delim, &size);

                                // impossible, parse opts should have exited
                                // if opt[0] -h: usage and exit
                                // if opt[0] -S: couldnt have converted 2nd opt 
                                //              opt to an integer, err and exit
                                // if opt[0] -D: it would hve taken the 2nd opt 
                                //              as its delim arg, BUT
                                // if opt[1] -D: then it would err and exit
                                // if none of these are true, 
                                //                but we received 2 mutex opts, 
                                //                would have exited.
                                if (opts[0][1] == 'h') 
                                    ASSERT_EQ_CHAR(i, j);
                                else if (opts[0][1] == 'S') 
                                    ASSERT_EQ_CHAR(i, j);
                                else if (opts[1][1] == 'D') 
                                    ASSERT_EQ_CHAR(i, j);
                                else if (is_mutex_opts(opts[0][1], opts[1][1]))
                                    ASSERT_EQ_CHAR(i, j);

                                // If none of the above are true, then we have 
                                //  not exited
                                // If opt[0] -D: it would have taken 2nd opt as
                                //              its own arg for delim, so 2nd
                                //              opt would not be processed as a
                                //              opt during flag calculation
                                // else business as usual
                                if (opts[0][1] == 'D') { 
                                    ASSERT_EQ(flags, (1<<DELIM));
                                    ASSERT_STREQ(delim, opts[1]);
                                } else {
                                    ASSERT_EQ(flags, ((1<<i) | (1<<j)));
                                    EXPECT_EQ_PTR(delim, NULL);
                                }
                                
                                // see "man 3 getopt"
                                EXPECT_EQ(optind, argc);
 
                                // if a type flag was selected -i -d -f -l and 
                                // processed (see -D eating up succeeding 
                                // option above) we would expect the size to be
                                // initialized to the correct sizeof(type)
                                // else DEFAULT_SIZE (ie sizeof(char))
                                if (flags & (1<<INT))     
                                    EXPECT_EQ(size, sizeof(int));
                                else if (flags & (1<<DOUBLE))  
                                    EXPECT_EQ(size, sizeof(double));
                                else if (flags & (1<<FLOAT))   
                                    EXPECT_EQ(size, sizeof(float));
                                else if (flags & (1<<LONG))    
                                    EXPECT_EQ(size, sizeof(long));
                                else
                                    EXPECT_EQ(size, DEFAULT_SIZE);
                            } 

                            char  *exp_err = NULL;
                            int   exp_exit = EXIT_FAILURE;  

                            if (opts[0][1] == 'h') {
                                easeprintf(&exp_err, UsageInfoStr);
                                exp_exit = EXIT_SUCCESS;
                            } 
                            else if (opts[0][1] == 'D')
                                exp_exit = EXIT_SUCCESS;
                            else if (opts[0][1] == 'S')
                                easeprintf(&exp_err, CantConvert, opts[1]);
                            else if (opts[1][1] == 'S')
                                easeprintf(&exp_err, OptReqsArg, 'S');
                            else if (is_mutex_opts(opts[0][1], opts[1][1])) 
                                easeprintf(&exp_err, InvOptMutex, MutexOpts);
                            else if (opts[1][1] == 'h') {
                                easeprintf(&exp_err, UsageInfoStr);
                                exp_exit = EXIT_SUCCESS;
                            } 
                            else if (opts[1][1] == 'D') 
                                easeprintf(&exp_err, OptReqsArg, 'D');
                            else 
                                exp_exit = EXIT_SUCCESS;
                            
                            EXPECT_OUT_EQ("");
                            EXPECT_SIGNAL_CODE_EQ(NOT_SIGNALED);

                            EXPECT_ERR_EQ((exp_err ? exp_err : ""));
                            EXPECT_EXIT_CODE_EQ(exp_exit);

                            if (exp_err) {
                                free(exp_err);
                                exp_err = NULL;
                            }

                        }
                }
            }
            TEST("test concat combined short opts") {

            }
            TEST("test single long  opts") {

            }
        }
    } 
//    test_parse_opts_short_combined_single();
//    test_parse_opts_short_combined_concat();
//    test_parse_opts_long_single();
//    test_parse_opts_long_combined();
//    test_parse_opts_short_long_mixed();
//    test_parse_opts_delim();
 
    return 0;
}

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

char **create_argv(int argc, ...)
{
   int i;
   char **argv;
   va_list args;

   argv = (char **) emalloc(sizeof(char *) * (argc+1));

   va_start(args, argc);
   for (i = 0; i < argc; i++)
       argv[i] = va_arg(args, char *);
   va_end(args);
   
   argv[argc] = NULL;
   return argv;
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

