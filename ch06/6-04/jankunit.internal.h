#ifndef JANKUNIT_INTERNAL_H
#define JANKUNIT_INTERNAL_H

#include <stdio.h>
#include <string.h>

typedef struct {
    char *test_name;
    int total;
    int passed;
    int failed;
    int failed_assert;
} Test;

typedef struct {
    char *suite_name;
    int total;
    int passed;
    int failed;
    int failed_assert;
} TestSuite;

typedef struct {
    char *program_name;
    int total;
    int passed;
    int failed;
    int failed_assert;
} TestProgram;

//TODO: move to a CTX based design
typedef struct {
    int         indent_level;
    TestProgram **progs;
    TestSuite   **suites;
    Test        **tests;

    TestProgram *current_program;
    TestSuite   *current_suite;
    Test        *current_test;
} JankUnitContext;

extern JankUnitContext *ctx;

extern int          ident_level;
extern TestProgram  *current_program;
extern TestSuite    *current_suite;
extern Test         *current_test;

extern char *OUT;
extern char *ERR;
extern int  STATUS;
extern int  EXIT_CODE;
extern int  SIGNAL_CODE;

void print_with_indent  (const char *fmt, ...);
void vprint_with_indent (const char *fmt, va_list args);
void print_result       (const char *color, const char *fmt, ...);
void vprint_result      (const char *color, const char *fmt, va_list args);

#endif
