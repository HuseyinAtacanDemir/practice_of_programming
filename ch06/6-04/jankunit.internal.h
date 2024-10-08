#ifndef JANKUNIT_INTERNAL_H
#define JANKUNIT_INTERNAL_H

#include <stdio.h>
#include <string.h>

typedef struct {
    const char *test_name;
    int passed;
    int failed;
} Test;

typedef struct {
    const char *subsection_name;
    int total_tests;
    int passed;
    int failed;
} TestSuite;

typedef struct {
    const char *program_name;
    int total_suites;
    int passed;
    int failed;
} TestProgram;

extern int indent_level;
extern TestProgram *current_program;
extern TestSuite *current_suite;
extern Test *current_test;

void print_with_indent  (const char *fmt, ...);
void print_result       (const char *color, const char *fmt, int pass, int fail);

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

// Integer equality comparison
#define EXPECT_EQ(val1, val2) \
    do { \
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
        int a = (val1); \
        int b = (val2); \
        if (a != b) { \
            print_with_indent("ASSERT failed: Expected %d, but got %d, %s == %s\n", b, a, #val1, #val2); \
            FAIL(); \
            return; \
        } else { \
            PASS(); \
        } \
    } while (0)

// Integer inequality comparison
#define EXPECT_NE(val1, val2) \
    do { \
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
        int a = (val1); \
        int b = (val2); \
        if (a == b) { \
            print_with_indent("ASSERT failed: Expected different from %d, but got %d, %s != %s\n", b, a, #val1, #val2); \
            FAIL(); \
            return; \
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

#define EXPECT_LT(val1, val2) \
    do { \
        int a = (val1); \
        int b = (val2); \
        if (a >= b) { \
            print_with_indent("Expected %d to be less than %d, %s < %s\n", a, b, #val1, #val2); \
            FAIL(); \
        } else { \
            PASS(); \
        } \
    } while (0)

#define EXPECT_GTE(val1, val2) \
    do { \
        int a = (val1); \
        int b = (val2); \
        if (a < b) { \
            print_with_indent("Expected %d to be greater than or equal to %d, %s >= %s\n", a, b, #val1, #val2); \
            FAIL(); \
        } else { \
            PASS(); \
        } \
    } while (0)

#define EXPECT_LTE(val1, val2) \
    do { \
        int a = (val1); \
        int b = (val2); \
        if (a > b) { \
            print_with_indent("Expected %d to be less than or equal to %d, %s <= %s\n", a, b, #val1, #val2); \
            FAIL(); \
        } else { \
            PASS(); \
        } \
    } while (0)

// String equality comparison (handles NULL pointers)
#define EXPECT_STREQ(str1, str2) \
    do { \
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
        const char* s1 = (str1); \
        const char* s2 = (str2); \
        if (!((s1 == NULL && s2 == NULL) || (s1 != NULL && s2 != NULL && strcmp(s1, s2) == 0))) { \
            print_with_indent("ASSERT failed: Expected string '%s', but got '%s'\n", (s2 ? s2 : "NULL"), (s1 ? s1 : "NULL")); \
            FAIL(); \
            return; \
        } else { \
            PASS(); \
        } \
    } while (0)

// Boolean assertions
#define EXPECT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            print_with_indent("Expected TRUE, but got FALSE, %s\n", #expr); \
            FAIL(); \
        } else { \
            PASS(); \
        } \
    } while (0)

#define EXPECT_FALSE(expr) \
    do { \
        if ((expr)) { \
            print_with_indent("Expected FALSE, but got TRUE, %s\n", #expr); \
            FAIL(); \
        } else { \
            PASS(); \
        } \
    } while (0)

#define ASSERT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            print_with_indent("ASSERT failed: Expected TRUE, but got FALSE, %s\n", #expr); \
            FAIL(); \
            return; \
        } else { \
            PASS(); \
        } \
    } while (0)

#define ASSERT_FALSE(expr) \
    do { \
        if ((expr)) { \
            print_with_indent("ASSERT failed: Expected FALSE, but got TRUE, %s\n", #expr); \
            FAIL(); \
            return; \
        } else { \
            PASS(); \
        } \
    } while (0)

#endif
