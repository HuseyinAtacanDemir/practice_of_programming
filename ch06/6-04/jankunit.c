#include "jankunit.h"
#include "jankunit.internal.h"

#include "eprintf.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define COLOR_GREEN "\x1b[32m"
#define COLOR_RED "\x1b[31m"
#define COLOR_RESET "\x1b[0m"

// Global test state
JankUnitContext *ctx = NULL;
TestProgram *current_program = NULL;
TestSuite *current_suite = NULL;
Test *current_test = NULL;

int STATUS = 0;
char *OUT = NULL;
char *ERR = NULL;
int *EXIT_CODE = NULL;
int *SIGNAL_CODE = NULL;

// Helper function for printing with indentation
void print_with_indent(const char *fmt, ...) 
{
    va_list args;
    va_start(args, fmt);

    for (int i = 0; i < indent_level; i++)
        printf("    ");  // Four spaces per level

    vprintf(fmt, args);
    va_end(args);
}

void vprint_with_indent(const char *fmt, va_list args)
{
    for (int i = 0; i < indent_level; i++)
        printf("    ");
    vprintf(fmt, args);
}

// Helper to print result with color
void print_result(const char *color, const char *fmt, ...) 
{
    va_list args;
    va_start(args, fmt);

    printf("%s", color);
    vprint_with_indent(fmt, args);
    printf("%s", COLOR_RESET);

    va_end(args);
}

void vprint_result(const char *color, const char *fmt, va_list args)
{
    printf("%s", color);
    vprint_with_indent(fmt, args);
    printf("%s", COLOR_RESET);
}

void start_test_program(const char *name_fmt, ...) 
{
    va_list args;

    current_program = (TestProgram *) emalloc(sizeof(TestProgram));
    current_program->total = 0;
    current_program->passed = 0;
    current_program->failed = 0;
    current_program->failed_assert = 0;

    va_start(args, name_fmt);
    evasprintf(&(current_program->program_name), name_fmt, args);
    va_end(args);

    print_with_indent("Running tests for: %s\n", current_program->program_name);
    indent_level++;
}

void end_test_program() 
{
    indent_level--;
    if (current_program->failed == 0) {
        print_result(COLOR_GREEN, "Total: %d, %d passed %d failed\n", 
          current_program->total, current_program->passed, current_program->failed);
    } else {
        print_result(COLOR_RED, "Total: %d, %d passed %d failed\n", 
          current_program->total, current_program->passed, current_program->failed);
    }

    free(current_program->program_name);
    free(current_program);
    current_program = NULL;
}

void start_test_suite(const char *name_fmt, ...) 
{
    va_list args;

    current_suite = (TestSuite *) emalloc(sizeof(TestSuite));
    current_suite->total = 0;
    current_suite->passed = 0;
    current_suite->failed = 0;
    current_suite->failed_assert = 0;

    va_start(args, name_fmt);
    evasprintf(&(current_suite->suite_name), name_fmt, args);
    va_end(args);

    print_with_indent("test suite: %s\n", current_suite->suite_name);
    indent_level++;
}

void end_test_suite() 
{
    int is_suite_failed = 0;
    indent_level--;
    if (current_suite->failed == 0) {
        print_result(COLOR_GREEN, "Total: %d, %d passed %d failed\n", 
          current_suite->total, current_suite->passed, current_suite->failed);
    } else {
        is_suite_failed = 1;
        print_result(COLOR_RED, "Total: %d, %d passed %d failed\n", 
          current_suite->total, current_suite->passed, current_suite->failed);
    }

    free(current_suite->suite_name);
    free(current_suite);
    current_suite = NULL;

    if (is_suite_failed)
        FAIL();
    else
        PASS();
}

void start_test(const char *name_fmt, ...) 
{
    va_list args;

    current_test = (Test *) emalloc(sizeof(Test));
    current_test->total = 0;
    current_test->passed = 0;
    current_test->failed = 0;
    current_test->failed_assert = 0;

    va_start(args, name_fmt);
    evasprintf(&(current_test->test_name), name_fmt, args);
    va_end(args);

    print_with_indent("test: %s\n", current_test->test_name);
    indent_level++;
}

void end_test() 
{
    int is_test_failed = 0;

    indent_level--;
    if (current_test->failed == 0) {
        print_result(COLOR_GREEN, "Total: %d, %d passed %d failed\n", 
          current_test->total, current_test->passed, current_test->failed);
    } else {
        is_test_failed = 1;
        print_result(COLOR_RED, "Total: %d, %d passed %d failed\n", 
          current_test->total, current_test->passed, current_test->failed);
    }     

    free(current_test->test_name);
    free(current_test);
    current_test = NULL;

    if (is_test_failed)
        FAIL();
    else
        PASS();
}


//void test_forked_fn_with_shared_mem()
//{
//
    //int shmem_size = 1024;
    //void *shmem;
//
    //shmem = mmap(NULL, shmem_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, 0, 0);
    //if (shmem == MAP_FAILED)
        //return NULL;
//
//}


