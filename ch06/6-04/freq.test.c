#include "freq.internal.h"
#include "freq.test_helper.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#include "jankunit.h"
#include "eprintf.h"

int main(void)
{
    init_ctx();
    TEST_PROGRAM("FREQ INTERNAL UNIT TESTS") {
/*
      TEST_SUITE("SUITE: atoi_pos") {
        struct {
            char *test_name;
            char *input;
            int exp_result;
        } cases[] = {
            {"test positive int str",   "123",          123},
            {"test int max str",        "2147483647",   INT_MAX},
            {"test zero int str",       "0",            NOT_POSITIVE_INT_ERR},
            {"test int min str",        "-2147483648",  NOT_POSITIVE_INT_ERR},
            {"test negative int str",   "-123",         NOT_POSITIVE_INT_ERR},
            {"test invalid str",        "asd",          NOT_POSITIVE_INT_ERR},
            {"test semivalid int str",  "123.123",      NOT_POSITIVE_INT_ERR},
            {"test int max+1 str",      "2147483648",   NOT_POSITIVE_INT_ERR},
            {"test int min-1 str",      "-2147483649",  NOT_POSITIVE_INT_ERR},
            {"test positive long str",  "3147483649",   NOT_POSITIVE_INT_ERR},
            {"test negative long str",  "-3147483649",  NOT_POSITIVE_INT_ERR},
            {"test stress",  "3149475927592457483649",  NOT_POSITIVE_INT_ERR},
            {NULL, NULL, 0}
        };
        for (int i = 0; cases[i].test_name; i++) {
            TEST(cases[i].test_name) {
                int result = atoi_pos(cases[i].input);
                EXPECT_EQ(result, cases[i].exp_result);
            }
        }
      }

      TEST_SUITE("SUITE: set_opt_flag") {
        TEST("test valid opts 0x0 initial flags") {
            for (int i = 0; i < N_SUPPORTED_OPTS; i++) {
                char opt = LongOpts[i].val; 
                EXPECT_EQ(set_opt_flag(0x0, opt), (1<<i));
            }
        }
        TEST("test valid opts non-zero initial flags") {
            int flag = (1 << N_SUPPORTED_OPTS); // LO unsupported bit set
            int old_flag = flag;
            for (int i = 0; i < N_SUPPORTED_OPTS; i++) {
                char opt = LongOpts[i].val;
                flag = set_opt_flag(flag, opt);
                EXPECT_EQ(flag, (old_flag | (1<<i)) );
                old_flag = flag;
            }
        }
        TEST("test invalid opts 0x0 initial flags") {
            int inv_opts[] = {'@', 'z', '1'};
            for (int i = 0; i < (sizeof(inv_opts)/sizeof(int)); i++) {
                EXPECT_EQ(set_opt_flag(0x0, inv_opts[i]), 0x0);
            }
        }
        TEST("test invalid opts non-zero initial flags") {
            int inv_opts[] = {'@', 'z', '1'};
            int flag = (1 << N_SUPPORTED_OPTS);
            int old_flag = flag;
            for (int i = 0; i < (sizeof(inv_opts)/sizeof(int)); i++) {
                flag = set_opt_flag(flag, inv_opts[i]);
                EXPECT_EQ(flag, old_flag);
                // old_flag = flag; no need, we EXPECT it not to change
            }
        }
        TEST("test boundaries INT_MAX INT_MIN UCHAR_MAX+1 CHAR_MIN-1") {
            int cases[] = { INT_MAX, INT_MIN, UCHAR_MAX+1, CHAR_MIN-1, 0 };
            for (int i = 0; cases[i] != 0; i++)
                EXPECT_EQ(0x0, set_opt_flag(0x0, cases[i]));
        }
      }
*/
      TEST_SUITE("SUITE: parse_opts") {
        struct {
            char  *name;
            int   argc;
            int   exp_flags;
            int   exp_size;
            char  *exp_delim;
            char  *exp_err;
            int   exp_exit_code;
            char  **argv;
        } cases[] = {
            // no opts
            /* {"test no args should just exit from fork with success",
              1, 0x000, 0, NULL, NULL, 
              EXIT_SUCCESS, create_argv(1)},

            // single valid short opts
            {"test -h should print usage and exit from fork with success", 
              2, 0x000, 0, NULL, get_eprintf_str(UsageInfoStr), 
              EXIT_SUCCESS, create_argv(2, "-h")},

            {"test -a should set the proper flag", 
              2, 0x002, 0, NULL, NULL, 
              EXIT_SUCCESS, create_argv(2, "-a")},

            {"test -s should set the proper flag", 
              2, 0x004, 0, NULL, NULL, 
              EXIT_SUCCESS, create_argv(2, "-s")}, */

            {"test -D should set delim to ','", 
              3, 0x008, 0, ",", NULL, 
              EXIT_SUCCESS, create_argv(3, "-D", ",")},

            /* {"test -c should set the proper flag", 
              2, 0x020, 0, NULL, NULL,
              EXIT_SUCCESS, create_argv(2, "-c")},

            {"test -i should set the proper flag", 
              2, 0x040, 0, NULL, NULL, 
              EXIT_SUCCESS, create_argv(2, "-i")},

            {"test -d should set the proper flag", 
              2, 0x080, 0, NULL, NULL, 
              EXIT_SUCCESS, create_argv(2, "-d")},
              
            {"test -S should set the proper flag", 
              2, 0x100, 0, NULL, NULL, 
              EXIT_SUCCESS, create_argv(2, "-S")}, */

            // single invalid usage short opts
            {"test -D should exit with failure and ErrOptReqsArg", 
              2, 0x000, 0, NULL, get_eprintf_str(ErrOptReqsArg, 'D'), 
              EXIT_SUCCESS, create_argv(2, "-D")},

            /* {"test -R should exit with failure and ErrOptMutexDefault", 
              2, 0x000, 0, NULL, get_eprintf_str(ErrOptMutexDefault), 
              EXIT_FAILURE, create_argv(2, "-R")},

            {"test -Rasd should exit with failure and ErrInvSizeArg", 
              2, 0x000, 0, NULL, get_eprintf_str(ErrInvSizeArg, "asd"), 
              EXIT_FAILURE, create_argv(2, "-Rasd")},

            // single invalid short opts
            {"test -z should exit with failure and ErrInvOpt", 
              2, 0x000, 0, NULL, get_eprintf_str(ErrInvOpt, "-z"), 
              EXIT_FAILURE, create_argv(2, "-z")},

            {"test -@ should exit with failure and ErrInvOpt", 
              2, 0x000, 0, NULL, get_eprintf_str(ErrInvOpt, "-@"), 
              EXIT_FAILURE, create_argv(2, "-@")}, */
            
            //{"test ",    
              //1, 0x000, 0, NULL, NULL, 
              //EXIT_SUCCESS, create_argv(1)},
            //{"test",    
              //1, 0x000, 0, NULL, NULL, 
              //EXIT_SUCCESS, create_argv(1)},
            //{"test",    
              //1, 0x000, 0, NULL, NULL, 
              //EXIT_SUCCESS, create_argv(1)},
            //{"test",    
              //1, 0x000, 0, NULL, NULL, 
              //EXIT_SUCCESS, create_argv(1)},
            //{"test",    
              //1, 0x000, 0, NULL, NULL, 
              //EXIT_SUCCESS, create_argv(1)},
            //{"test",    
              //1, 0x000, 0, NULL, NULL, 
              //EXIT_SUCCESS, create_argv(1)},
            {NULL, 0, 0, 0, 0, 0, 0, NULL}
        };

        for (int i = 0; cases[i].name; i++) {

            TEST(cases[i].name) {
                FORK(){ // jankunit FORKs exit at the end of block with 0
                    char *delim = NULL;
                    int  size   = 0;    
                    optind = optreset = 1;
                    int  flags  = parse_opts(cases[i].argc, cases[i].argv,
                                            &delim, &size);

                    if (cases[i].exp_exit_code != EXIT_SUCCESS) ASSERT_TRUE(0);

                    EXPECT_EQ(optind, cases[i].argc);
                    EXPECT_EQ(flags, cases[i].exp_flags);
                    EXPECT_EQ(size, cases[i].exp_size);
                    if (cases[i].exp_delim) 
                        EXPECT_STREQ(delim, cases[i].exp_delim);
                    else
                        EXPECT_EQ_PTR(delim, cases[i].exp_delim);
                }

                EXPECT_EXIT_CODE_EQ(cases[i].exp_exit_code);
                EXPECT_SIGNAL_CODE_EQ(NOT_SIGNALED);
                EXPECT_OUT_EQ("");

                if (cases[i].exp_err == NULL) {
                    EXPECT_ERR_EQ("");
                } else { 
                    EXPECT_ERR_EQ(cases[i].exp_err);
                    free(cases[i].exp_err);
                    cases[i].exp_err = NULL;
                }
            }
            destroy_argv(cases[i].argv);
        }
/*
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
        TEST("test concat combined short opts w\\o requirements") {
            for (int i = 0; i < N_SUPPORTED_OPTS-1; i++) {
                if (LongOpts[i].has_arg == required_argument)
                    continue;
                for (int j = i+1; j < N_SUPPORTED_OPTS; j++) {
                    if (LongOpts[j].has_arg == required_argument)
                        continue;
                    int argc = 2;
                    char *opts = NULL;
                    easprintf(&opts, "-%c%c", LongOpts[i].val, LongOpts[j].val);
                    FORK() {
                        int size = DEFAULT_SIZE;
                        char *delim = DEFAULT_DELIM;
                        char **argv = create_argv(2, "./freq_test", opts);
                        
                        int flags = parse_opts(argc, argv, &delim, &size);

                        if (index(opts, 'h')) 
                            ASSERT_TRUE(0);
                        else if (is_mutex_opts(opts[1], opts[2]))
                            ASSERT_TRUE(0);
                        
                        EXPECT_EQ(optind, argc);
                        EXPECT_EQ_PTR(delim, NULL);
                        EXPECT_EQ(flags, ((1<<i)|(1<<j)));

                        if (index(opts, 'i'))
                            EXPECT_EQ(size, sizeof(int));
                        else if (index(opts, 'd'))
                            EXPECT_EQ(size, sizeof(double));
                        else if (index(opts, 'f'))
                            EXPECT_EQ(size, sizeof(float));
                        else if (index(opts, 'l'))
                            EXPECT_EQ(size, sizeof(long));
                        else 
                            EXPECT_EQ(size, sizeof(char));
                    }
                  
                    EXPECT_OUT_EQ("");
                    EXPECT_SIGNAL_CODE_EQ(NOT_SIGNALED);

                    char *exp_err = NULL;
                    int exp_exit = EXIT_SUCCESS; 

                    if (index(opts, 'h')) { 
                        easeprintf(&exp_err, UsageInfoStr);
                    } else if (is_mutex_opts(opts[1], opts[2])) {
                        easeprintf(&exp_err, InvOptMutex, MutexOpts);
                        exp_exit = EXIT_FAILURE;
                    }

                    EXPECT_ERR_EQ((exp_err ? exp_err : ""));
                    EXPECT_EXIT_CODE_EQ(exp_exit);
                    
                    free(opts);
                }
            }
        }
        TEST("test concat combined short opts with requirements") {
            int argc;
            FORK() {
                argc = 2;
                char **argv = create_argv(argc, "./freq_test", "-RS5");
                int size = 0;
                char *delim = NULL;
                int flags = parse_opts(2, argv, &delim, &size);
                EXPECT_EQ(optind, argc);
                EXPECT_EQ_PTR(delim, NULL);
                EXPECT_EQ(flags, ((1<<RAW)|(1<<STRUCT)));
                EXPECT_EQ(size, 5);
            }
            EXPECT_OUT_EQ("");
            EXPECT_ERR_EQ("");
            EXPECT_EXIT_CODE_EQ(EXIT_SUCCESS);
            EXPECT_SIGNAL_CODE_EQ(NOT_SIGNALED);
        }
        TEST("test single long opts") {

        }

*/
      }
    } 
    //eshfree_all();
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
