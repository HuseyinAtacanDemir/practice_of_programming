#include "jankunit.h"
#include "eprintf.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define COLOR_GREEN "\x1b[32m"
#define COLOR_RED "\x1b[31m"
#define COLOR_RESET "\x1b[0m"

// Global test state
int indent_level = 0;
TestProgram *current_program = NULL;
TestSuite *current_suite = NULL;
Test *current_test = NULL;

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

// Helper to print result with color
void print_result(const char *color, const char *fmt, int pass, int fail) 
{
    
    printf("%s", color);
    print_with_indent(fmt, pass, fail);
    printf("%s", COLOR_RESET);

}

void start_test_program(const char *name) 
{
    current_program = (TestProgram *) emalloc(sizeof(TestProgram));
    current_program->program_name = name;
    current_program->total_suites = 0;
    current_program->passed = 0;
    current_program->failed = 0;

    print_with_indent("Running tests for: %s\n", name);
    indent_level++;
}

void end_test_program() 
{
    indent_level--;
    if (current_program->failed == 0) {
        print_result(COLOR_GREEN, "Total: %d passed %d failed\n", 
                      current_program->passed, current_program->failed);
    } else {
        print_result(COLOR_RED, "Total: %d passed %d failed\n", 
                      current_program->passed, current_program->failed);
    }

    free(current_program);
    current_program = NULL;
}

void start_test_suite(const char *name) 
{
    current_suite = (TestSuite *) emalloc(sizeof(TestSuite));
    current_suite->subsection_name = name;
    current_suite->total_tests = 0;
    current_suite->passed = 0;
    current_suite->failed = 0;

    print_with_indent("test suite: %s\n", name);
    indent_level++;
}

void end_test_suite() 
{
    indent_level--;
    if (current_suite->failed == 0) {
        print_result(COLOR_GREEN, "Total: %d passed %d failed\n", 
                      current_suite->passed, current_suite->failed);
    } else {
        print_result(COLOR_RED, "Total: %d passed %d failed\n", 
                      current_suite->passed, current_suite->failed);
    }

    free(current_suite);
    current_suite = NULL;
}

void start_test(const char *name) 
{
    current_test = (Test *) emalloc(sizeof(Test));
    current_test->test_name = name;
    current_test->passed = 0;
    current_test->failed = 0;

    print_with_indent("test: %s\n", name);
    indent_level++;
}

void end_test() 
{
    indent_level--;
    if (current_test->failed == 0) {
        print_result(COLOR_GREEN, "Total: %d passed %d failed\n", 
                      current_test->passed, current_test->failed);
    } else {
        print_result(COLOR_RED, "Total: %d passed %d failed\n", 
                      current_test->passed, current_test->failed);
    }

    free(current_test);
    current_test = NULL;
}

