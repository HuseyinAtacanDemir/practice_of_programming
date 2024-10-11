#ifndef JANKUNIT_ASSERT_H
#define JANKUNIT_ASSERT_H

#define PASS() \
    do { \
        if (current_test != NULL) { current_test->passed++; } \
        else if (current_suite != NULL) { current_suite->passed++; } \
        else if (current_program != NULL) { current_program->passed++; } \
    } while (0)

#define FAIL() \
    do {  \
        if (current_test != NULL) { current_test->failed++; } \
        else if (current_suite != NULL) { current_suite->failed++; } \
        else if (current_program != NULL) { current_program->failed++; } \
    } while (0)

#define FAIL_ASSERT() \
    do {  \
        if (current_test != NULL) { current_test->failed++; current_test->failed_assert++; } \
        else if (current_suite != NULL) { current_suite->failed++; current_suite->failed_assert++; } \
        else if (current_program != NULL) { current_program->failed++; current_program->failed_assert++; } \
    } while (0)

#define INCREMENT_TOTAL_IN_PARENT() \
    do {  \
        if (current_test != NULL) { current_test->total++; } \
        else if (current_suite != NULL) { current_suite->total++; }  \
        else if (current_program != NULL) { current_program->total++; }  \
    } while (0)

#define CONTINUE_IF_FAILED_ASSERT_IN_SCOPE() \
    if (current_test != NULL && current_test->failed_assert > 0)  \
        continue; \
    else if (current_suite != NULL && current_suite->failed_assert > 0) \
        continue; \
    else if (current_program != NULL && current_program->failed_assert > 0) \
        continue


// Integer equality comparison
#define EXPECT_EQ(val1, val2) \
    do { \
        INCREMENT_TOTAL_IN_PARENT();  \
        CONTINUE_IF_FAILED_ASSERT_IN_SCOPE(); \
        int a = (val1); \
        int b = (val2); \
        if (a != b) { \
            print_with_indent("Expected %d, but got %d, %s == %s\n", b, a, #val1, #val2); \
            FAIL(); \
        } else { \
            PASS(); \
        } \
    } while (0)

#define ASSERT_EQ(val1, val2) \
    do { \
        INCREMENT_TOTAL_IN_PARENT();  \
        CONTINUE_IF_FAILED_ASSERT_IN_SCOPE(); \
        int a = (val1); \
        int b = (val2); \
        if (a != b) { \
            print_with_indent("ASSERT failed: Expected %d, but got %d, %s == %s\n", b, a, #val1, #val2); \
            FAIL_ASSERT(); \
        } else { \
            PASS(); \
        } \
    } while (0)

// Integer inequality comparison
#define EXPECT_NE(val1, val2) \
    do { \
        INCREMENT_TOTAL_IN_PARENT();  \
        CONTINUE_IF_FAILED_ASSERT_IN_SCOPE(); \
        int a = (val1); \
        int b = (val2); \
        if (a == b) { \
            print_with_indent("Expected different from %d, but got %d, %s != %s\n", b, a, #val1, #val2); \
            FAIL(); \
        } else { \
            PASS(); \
        } \
    } while (0)

#define ASSERT_NE(val1, val2) \
    do { \
        INCREMENT_TOTAL_IN_PARENT();  \
        CONTINUE_IF_FAILED_ASSERT_IN_SCOPE(); \
        int a = (val1); \
        int b = (val2); \
        if (a == b) { \
            print_with_indent("ASSERT failed: Expected different from %d, but got %d, %s != %s\n", b, a, #val1, #val2); \
            FAIL_ASSERT(); \
        } else { \
            PASS(); \
        } \
    } while (0)

// Greater-than and less-than comparisons
#define EXPECT_GT(val1, val2) \
    do { \
        int a = (val1); \
        int b = (val2); \
        if (a <= b) { \
            print_with_indent("Expected %d to be greater than %d, %s > %s\n", a, b, #val1, #val2); \
            FAIL(); \
        } else { \
            PASS(); \
        } \
    } while (0)

#define ASSERT_GT(val1, val2) \
    do { \
        INCREMENT_TOTAL_IN_PARENT();  \
        CONTINUE_IF_FAILED_ASSERT_IN_SCOPE(); \
        int a = (val1); \
        int b = (val2); \
        if (a <= b) { \
            print_with_indent("ASSERT failed: Expected %d to be greater than %d, %s > %s\n", a, b, #val1, #val2); \
            FAIL_ASSERT(); \
        } else { \
            PASS(); \
        } \
    } while (0)

#define EXPECT_LT(val1, val2) \
    do { \
        INCREMENT_TOTAL_IN_PARENT();  \
        CONTINUE_IF_FAILED_ASSERT_IN_SCOPE(); \
        int a = (val1); \
        int b = (val2); \
        if (a >= b) { \
            print_with_indent("Expected %d to be less than %d, %s < %s\n", a, b, #val1, #val2); \
            FAIL(); \
        } else { \
            PASS(); \
        } \
    } while (0)

#define ASSERT_LT(val1, val2) \
    do { \
        INCREMENT_TOTAL_IN_PARENT();  \
        CONTINUE_IF_FAILED_ASSERT_IN_SCOPE(); \
        int a = (val1); \
        int b = (val2); \
        if (a >= b) { \
            print_with_indent("ASSERT failed: Expected %d to be less than %d, %s < %s\n", a, b, #val1, #val2); \
            FAIL_ASSERT(); \
        } else { \
            PASS(); \
        } \
    } while (0)

#define EXPECT_GTE(val1, val2) \
    do { \
        INCREMENT_TOTAL_IN_PARENT();  \
        CONTINUE_IF_FAILED_ASSERT_IN_SCOPE(); \
        int a = (val1); \
        int b = (val2); \
        if (a < b) { \
            print_with_indent("Expected %d to be greater than or equal to %d, %s >= %s\n", a, b, #val1, #val2); \
            FAIL(); \
        } else { \
            PASS(); \
        } \
    } while (0)

#define ASSERT_GTE(val1, val2) \
    do { \
        INCREMENT_TOTAL_IN_PARENT();  \
        CONTINUE_IF_FAILED_ASSERT_IN_SCOPE(); \
        int a = (val1); \
        int b = (val2); \
        if (a < b) { \
            print_with_indent("Assert failed: Expected %d to be greater than or equal to %d, %s >= %s\n", a, b, #val1, #val2); \
            FAIL_ASSERT(); \
        } else { \
            PASS(); \
        } \
    } while (0)

#define EXPECT_LTE(val1, val2) \
    do { \
        INCREMENT_TOTAL_IN_PARENT();  \
        CONTINUE_IF_FAILED_ASSERT_IN_SCOPE(); \
        int a = (val1); \
        int b = (val2); \
        if (a > b) { \
            print_with_indent("Expected %d to be less than or equal to %d, %s <= %s\n", a, b, #val1, #val2); \
            FAIL(); \
        } else { \
            PASS(); \
        } \
    } while (0)

#define ASSERT_LTE(val1, val2) \
    do { \
        INCREMENT_TOTAL_IN_PARENT();  \
        CONTINUE_IF_FAILED_ASSERT_IN_SCOPE(); \
        int a = (val1); \
        int b = (val2); \
        if (a > b) { \
            print_with_indent("ASSERT failed: Expected %d to be less than or equal to %d, %s <= %s\n", a, b, #val1, #val2); \
            FAIL_ASSERT(); \
        } else { \
            PASS(); \
        } \
    } while (0)

// String equality comparison (handles NULL pointers)
#define EXPECT_STREQ(str1, str2) \
    do { \
        INCREMENT_TOTAL_IN_PARENT();  \
        CONTINUE_IF_FAILED_ASSERT_IN_SCOPE(); \
        const char* s1 = (str1); \
        const char* s2 = (str2); \
        if (!((s1 == NULL && s2 == NULL) || (s1 != NULL && s2 != NULL && strcmp(s1, s2) == 0))) { \
            print_with_indent("Expected string '%s', but got '%s'\n", (s2 ? s2 : "NULL"), (s1 ? s1 : "NULL")); \
            FAIL(); \
        } else { \
            PASS(); \
        } \
    } while (0)

#define ASSERT_STREQ(str1, str2) \
    do { \
        INCREMENT_TOTAL_IN_PARENT();  \
        CONTINUE_IF_FAILED_ASSERT_IN_SCOPE(); \
        const char* s1 = (str1); \
        const char* s2 = (str2); \
        if (!((s1 == NULL && s2 == NULL) || (s1 != NULL && s2 != NULL && strcmp(s1, s2) == 0))) { \
            print_with_indent("ASSERT failed: Expected string '%s', but got '%s'\n", (s2 ? s2 : "NULL"), (s1 ? s1 : "NULL")); \
            FAIL_ASSERT(); \
        } else { \
            PASS(); \
        } \
    } while (0)

// Boolean assertions
#define EXPECT_TRUE(expr) \
    do { \
        INCREMENT_TOTAL_IN_PARENT();  \
        CONTINUE_IF_FAILED_ASSERT_IN_SCOPE(); \
        if (!(expr)) { \
            print_with_indent("Expected TRUE, but got FALSE, %s\n", #expr); \
            FAIL(); \
        } else { \
            PASS(); \
        } \
    } while (0)

#define ASSERT_TRUE(expr) \
    do { \
        INCREMENT_TOTAL_IN_PARENT();  \
        CONTINUE_IF_FAILED_ASSERT_IN_SCOPE(); \
        if (!(expr)) { \
            print_with_indent("ASSERT failed: Expected TRUE, but got FALSE, %s\n", #expr); \
            FAIL_ASSERT(); \
        } else { \
            PASS(); \
        } \
    } while (0)

#define EXPECT_FALSE(expr) \
    do { \
        INCREMENT_TOTAL_IN_PARENT();  \
        CONTINUE_IF_FAILED_ASSERT_IN_SCOPE(); \
        if ((expr)) { \
            print_with_indent("Expected FALSE, but got TRUE, %s\n", #expr); \
            FAIL(); \
        } else { \
            PASS(); \
        } \
    } while (0)

#define ASSERT_FALSE(expr) \
    do { \
        INCREMENT_TOTAL_IN_PARENT();  \
        CONTINUE_IF_FAILED_ASSERT_IN_SCOPE(); \
        if ((expr)) { \
            print_with_indent("ASSERT failed: Expected FALSE, but got TRUE, %s\n", #expr); \
            FAIL_ASSERT(); \
        } else { \
            PASS(); \
        } \
    } while (0)
/*
create the pipes
create the GLOBAL BUFS (OUT ERR)
create the GLOBAL STATUS

for the function being tested
within the forked block:
    run the assertions 
        ( what if the forked code exits so no assertions are run?
              In that case, the checks provded within the fork scope won't be run,
              and the user won't be notified the true number of total test cases,
              since the check macros (expect/assert_*) all update the counts of their
              scope as they run.

              should I create some logic for providing shmem vars prior to a fork
              and then make all checks be made in the parent?

        )

is mocking print and exit calls the true path perhaps..?

what if assertion fails inside the FORK scope? Does that clear the checks remaning
in the parent after the fork cleanup? 

No, after the fork cleanup, the tests that follow are
immediate to the status/system wide side effects of the fn being run, so the assert_failed status
wont be propagated to the parent, however total assert/expect count will be.


*/

#define FORK(fn, result, ...) \
    void *_dup_ctx_ = duplicate_context();  \
    int _pipe_out_[2], _pipe_err_[2]; \
    pipe(_pipe_out_); \
    pipe(_pipe_err_); \
    int _pid_ = fork(); \
    if(_pid_ == 0) {  \
        redirect2pipe(_pipe_out_, STDOUT_FILENO); \
        redirect2pipe(_pipe_err_, STDERR_FILENO); \
        result = fn(__VA_ARGS__); \
        for (int _fork_flag_ = 1; _fork_flag_; _fork_flag_--)

#define FORK_END  \
    } \
    sync_ctx(_dup_ctx_);  \
    close(pipe_out[1]); \
    close(pipe_err[1]); \
    read_pipe(&OUT, pipe_out);  \
    read_pipe(&ERR, pipe_err);  \
    close(pipe_out[0]); \
    close(pipe_err[0]); \
    wait(&STATUS)
    

#define FORK()  \
    for ( pid_t _pid_ = -2; fork_setup(); (cleanup() && wait(&STATUS)))  \
        for ( _pid_ = fork(); (_pid_ == 0 && redirect2pipe()); exit(0) )

#define TEST_PROGRAM(name_fmt, ...) \
    for (int _prog_flag_ = (start_test_program((name_fmt), __VA_ARGS__), 1); \
          _prog_flag_; \
          _prog_flag_ = (end_test_program(), 0) )

#define TEST_SUITE(name_fmt, ...) \
    for (int _suite_flag_ = (start_test_suite((name_fmt), __VA_ARGS__), 1); \
          _suite_flag_; \
          _suite_flag_ = (end_test_suite(), 0), \
          (current_program ? current_program->total++ : 0) )

#define TEST(name_fmt, ...) \
    for (int _test_flag_ = (start_test((name_fmt), __VA_ARGS__), 1); \
          _test_flag_; \
          _test_flag_ = (end_test(), 0),  \
          (current_suite ? current_suite->total++ : (current_program ? current_program->total++ : 0 )) )

#endif
