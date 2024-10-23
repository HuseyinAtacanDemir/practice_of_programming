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
      TEST_SUITE("SUITE: atoi_pos") {
        struct {
            char *test_name;
            char *input;
            int exp_result;
        } tests[] = {
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
        for (int i = 0; tests[i].test_name; i++) {
            TEST(tests[i].test_name) {
                int result = atoi_pos(tests[i].input);
                EXPECT_EQ(result, tests[i].exp_result);
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
            int tests[] = { INT_MAX, INT_MIN, UCHAR_MAX+1, CHAR_MIN-1, 0 };
            for (int i = 0; tests[i] != 0; i++)
                EXPECT_EQ(0x0, set_opt_flag(0x0, tests[i]));
        }
      }

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
        } tests[] = {
            // no opts
             {"test no args should just exit from fork with success",           1, 0x000, 0,  NULL, NULL,                                   EXIT_SUCCESS, create_argv(1)},
            // single valid short opts
            {"test -h should print usage and exit from fork with success",      2, 0x000, 0,  NULL, get_eprintf_str(UsageInfoStr),          EXIT_SUCCESS, create_argv(2, "-h")},
            {"test -a should set the proper flag",                              2, 0x002, 0,  NULL, NULL,                                   EXIT_SUCCESS, create_argv(2, "-a")},
            {"test -s should set the proper flag",                              2, 0x004, 0,  NULL, NULL,                                   EXIT_SUCCESS, create_argv(2, "-s")},
            {"test -D should set delim to ','",                                 3, 0x008, 0,   ",", NULL,                                   EXIT_SUCCESS, create_argv(3, "-D", ",")},
            {"test -R5 should set size to 5",                                   2, 0x010, 5,  NULL, NULL,                                   EXIT_SUCCESS, create_argv(2, "-R5")},
            {"test -c should set the proper flag",                              2, 0x020, 0,  NULL, NULL,                                   EXIT_SUCCESS, create_argv(2, "-c")},
            {"test -i should set the proper flag",                              2, 0x040, 0,  NULL, NULL,                                   EXIT_SUCCESS, create_argv(2, "-i")},
            {"test -d should set the proper flag",                              2, 0x080, 0,  NULL, NULL,                                   EXIT_SUCCESS, create_argv(2, "-d")},
            {"test -S should set the proper flag",                              2, 0x100, 0,  NULL, NULL,                                   EXIT_SUCCESS, create_argv(2, "-S")},
            // single invalid usage short opts
            {"test -D should exit with failure and ErrOptReqsArg",              2, 0x000, 0,  NULL, get_eprintf_str(ErrOptReqsArg, 'D'),    EXIT_FAILURE, create_argv(2, "-D")},
            {"test -R should exit with failure and ErrOptMutexDefault",         2, 0x000, 0,  NULL, get_eprintf_str(ErrOptMutexDefault),    EXIT_FAILURE, create_argv(2, "-R")},
            {"test -Rasd should exit with failure and ErrInvSizeArg",           2, 0x000, 0,  NULL, get_eprintf_str(ErrInvSizeArg, "asd"),  EXIT_FAILURE, create_argv(2, "-Rasd")},
            // single invalid short opts
            {"test --z should exit with failure and ErrInvOpt",                 2, 0x000, 0,  NULL, get_eprintf_str(ErrInvOpt, "-z"),       EXIT_FAILURE, create_argv(2, "-z")},
            {"test -@ should exit with failure and ErrInvOpt",                  2, 0x000, 0,  NULL, get_eprintf_str(ErrInvOpt, "-@"),       EXIT_FAILURE, create_argv(2, "-@")},
            // separate combined 2 valid short opts
            {"test -a -s should set proper flags",                              3, 0x006, 0,  NULL, NULL,                                   EXIT_SUCCESS, create_argv(3, "-a", "-s")},
            {"test -a -D . should set proper flags and delim",                  4, 0x00A, 0,   ".", NULL,                                   EXIT_SUCCESS, create_argv(4, "-a", "-D", ".")},
            {"test -s -R10 should set proper flags and size",                   3, 0x014, 10, NULL, NULL,                                   EXIT_SUCCESS, create_argv(3, "-s", "-R10")},
            {"test -s -c should set proper flags",                              3, 0x024, 0,  NULL, NULL,                                   EXIT_SUCCESS, create_argv(3, "-s", "-c")},
            {"test -D : -c should set proper flags and delim",                  4, 0x028, 0,   ":", NULL,                                   EXIT_SUCCESS, create_argv(4, "-D", ":", "-c")},
            {"test -D - -i should set proper flags and delim",                  4, 0x048, 0,   "-", NULL,                                   EXIT_SUCCESS, create_argv(4, "-D", "-", "-i")},
            {"test -R -i should set proper flags",                              3, 0x050, 0,  NULL, NULL,                                   EXIT_SUCCESS, create_argv(3, "-R", "-i")},
            {"test -R -d should set proper flags and delim",                    3, 0x090, 0,  NULL, NULL,                                   EXIT_SUCCESS, create_argv(3, "-R", "-d")},
            // separate combined 2 invalid usage short opts
            {"test -s -R should exit with failure",                             3, 0x000, 0,  NULL, get_eprintf_str(ErrOptMutexDefault),    EXIT_FAILURE, create_argv(3, "-s", "-R")},
            {"test -c -R10 should exit with failure",                           3, 0x000, 0,  NULL, get_eprintf_str(ErrMultiSizeRaw),       EXIT_FAILURE, create_argv(3, "-c", "-R10")},
            {"test -s -R should exit with failure",                             3, 0x000, 0,  NULL, get_eprintf_str(ErrOptMutexDefault),    EXIT_FAILURE, create_argv(3, "-s", "-R")},
            {"test -R5 -R10 should exit with failure",                          3, 0x000, 0,  NULL, get_eprintf_str(ErrDupOptArg, 'R'),     EXIT_FAILURE, create_argv(3, "-R5", "-R10")},
            {"test -S -R10 should exit with failure",                           3, 0x000, 0,  NULL, get_eprintf_str(ErrOptMutex, 'S', 'R'), EXIT_FAILURE, create_argv(3, "-S", "-R10")},
            {"test -D * -R10 should exit with failure",                         4, 0x000, 0,  NULL, get_eprintf_str(ErrOptMutex, 'D', 'R'), EXIT_FAILURE, create_argv(4, "-D", "*", "-R10")},
            {"test -D - -D + should exit with failure",                         5, 0x000, 0,  NULL, get_eprintf_str(ErrDupOptArg, 'D'),     EXIT_FAILURE, create_argv(5, "-D", "-", "-D", "+")},
            // single valid long opts
            {"test --help should print usage and exit from fork with success",  2, 0x000, 0,  NULL, get_eprintf_str(UsageInfoStr),          EXIT_SUCCESS, create_argv(2, "--help")},
            {"test --aggregate should set the proper flag",                     2, 0x002, 0,  NULL, NULL,                                   EXIT_SUCCESS, create_argv(2, "--aggregate")},
            {"test --sort should set the proper flag",                          2, 0x004, 0,  NULL, NULL,                                   EXIT_SUCCESS, create_argv(2, "--sort")},
            {"test --delim=, should set delim to ','",                          2, 0x008, 0,   ",", NULL,                                   EXIT_SUCCESS, create_argv(2, "--delim=,")},
            {"test --raw=5 should set size to 5",                               2, 0x010, 5,  NULL, NULL,                                   EXIT_SUCCESS, create_argv(2, "--raw=5")},
            {"test --char should set the proper flag",                          2, 0x020, 0,  NULL, NULL,                                   EXIT_SUCCESS, create_argv(2, "--char")},
            {"test --int should set the proper flag",                           2, 0x040, 0,  NULL, NULL,                                   EXIT_SUCCESS, create_argv(2, "--int")},
            {"test --double should set the proper flag",                        2, 0x080, 0,  NULL, NULL,                                   EXIT_SUCCESS, create_argv(2, "--double")},
            {"test --string should set the proper flag",                        2, 0x100, 0,  NULL, NULL,                                   EXIT_SUCCESS, create_argv(2, "--string")},
            // single invalid usage long opts
            {"test --delim should exit with failure and ErrOptReqsArg",         2, 0x000, 0,  NULL, get_eprintf_str(ErrOptReqsArg, 'D'),    EXIT_FAILURE, create_argv(2, "--delim")},
            {"test --raw should exit with failure and ErrOptMutexDefault",      2, 0x000, 0,  NULL, get_eprintf_str(ErrOptMutexDefault),    EXIT_FAILURE, create_argv(2, "--raw")},
            {"test --raw=asd should exit with failure and ErrInvSizeArg",       2, 0x000, 0,  NULL, get_eprintf_str(ErrInvSizeArg, "asd"),  EXIT_FAILURE, create_argv(2, "--raw=asd")},
            // single invalid long opts
            {"test --zafga should exit with failure and ErrInvOpt",             2, 0x000, 0,  NULL, get_eprintf_str(ErrInvOpt, "--zafga"),  EXIT_FAILURE, create_argv(2, "--zafga")},
            {"test --@+&123 should exit with failure and ErrInvOpt",            2, 0x000, 0,  NULL, get_eprintf_str(ErrInvOpt, "--@+&123"), EXIT_FAILURE, create_argv(2, "--@+&123")},
            // separate combined 2 valid long opts
            {"test --aggregate --sort should set proper flags",                 3, 0x006, 0,  NULL, NULL,                                   EXIT_SUCCESS, create_argv(3, "--aggregate", "--sort")},
            {"test --aggregate --delim=. should set proper flags and delim",    3, 0x00A, 0,   ".", NULL,                                   EXIT_SUCCESS, create_argv(3, "--aggregate", "--delim=.")},
            {"test --sort --raw=10 should set proper flags and size",           3, 0x014, 10, NULL, NULL,                                   EXIT_SUCCESS, create_argv(3, "--sort", "--raw=10")},
            {"test --sort --char should set proper flags",                      3, 0x024, 0,  NULL, NULL,                                   EXIT_SUCCESS, create_argv(3, "--sort", "--char")},
            {"test --delim=: --char should set proper flags and delim",         3, 0x028, 0,   ":", NULL,                                   EXIT_SUCCESS, create_argv(3, "--delim=:", "--char")},
            {"test --delim=- --int should set proper flags and delim",          3, 0x048, 0,   "-", NULL,                                   EXIT_SUCCESS, create_argv(3, "--delim=-", "--int")},
            {"test --raw --int should set proper flags",                        3, 0x050, 0,  NULL, NULL,                                   EXIT_SUCCESS, create_argv(3, "--raw", "--int")},
            {"test --raw --double should set proper flags and delim",           3, 0x090, 0,  NULL, NULL,                                   EXIT_SUCCESS, create_argv(3, "--raw", "--double")},
            // separate combined 2 invalid usage long opts
            {"test --sort --raw should exit with failure",                      3, 0x000, 0,  NULL, get_eprintf_str(ErrOptMutexDefault),    EXIT_FAILURE, create_argv(3, "--sort", "--raw")},
            {"test --char --raw=10 should exit with failure",                   3, 0x000, 0,  NULL, get_eprintf_str(ErrMultiSizeRaw),       EXIT_FAILURE, create_argv(3, "--char", "--raw=10")},
            {"test --sort --raw should exit with failure",                      3, 0x000, 0,  NULL, get_eprintf_str(ErrOptMutexDefault),    EXIT_FAILURE, create_argv(3, "--sort", "--raw")},
            {"test --raw=5 --raw=10 should exit with failure",                  3, 0x000, 0,  NULL, get_eprintf_str(ErrDupOptArg, 'R'),     EXIT_FAILURE, create_argv(3, "--raw=5", "--raw=10")},
            {"test --string --raw=10 should exit with failure",                 3, 0x000, 0,  NULL, get_eprintf_str(ErrOptMutex, 'S', 'R'), EXIT_FAILURE, create_argv(3, "--string", "--raw=10")},
            {"test --delim=* --raw=10 should exit with failure",                3, 0x000, 0,  NULL, get_eprintf_str(ErrOptMutex, 'D', 'R'), EXIT_FAILURE, create_argv(3, "--delim=*", "--raw=10")},
            {"test --delim=- --delim=+ should exit with failure",               3, 0x000, 0,  NULL, get_eprintf_str(ErrDupOptArg, 'D'),     EXIT_FAILURE, create_argv(3, "--delim=-", "--delim=+")},
            {NULL, 0, 0, 0, 0, 0, 0, NULL}
        };

        for (int i = 0; tests[i].name; i++) {
            TEST(tests[i].name) {
                FORK(){ // jankunit FORKs exit at the end of block with 0
                    int   argc    = tests[i].argc;
                    char  **argv  = tests[i].argv; 
                    char  *delim  = NULL;
                    int   size    = 0;

                    optind = optreset = 1;
                    int flags = parse_opts(argc, argv, &delim, &size);

                    if (tests[i].exp_exit_code != EXIT_SUCCESS) ASSERT_TRUE(0);

                    EXPECT_EQ(optind, argc);
                    EXPECT_EQ(flags, tests[i].exp_flags);
                    EXPECT_EQ_STR(delim, tests[i].exp_delim);
                    EXPECT_EQ(size, tests[i].exp_size);
                }

                EXPECT_EXIT_CODE_EQ(tests[i].exp_exit_code);
                EXPECT_SIGNAL_CODE_EQ(NOT_SIGNALED);

                EXPECT_OUT_EQ(NULL);
                EXPECT_ERR_EQ(tests[i].exp_err);
                
                if (tests[i].exp_err != NULL) {
                    free(tests[i].exp_err);
                    tests[i].exp_err = NULL;
                }
            }
            destroy_argv(tests[i].argv);
            tests[i].argv = NULL;
        }
      }
    } 
    return 0;
}
