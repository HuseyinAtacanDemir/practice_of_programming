#ifndef JANKUNIT_INTERNAL_H
#define JANKUNIT_INTERNAL_H

#include <stdio.h>
#include <string.h>

enum { USR, SYS };
enum { OUT, ERR };
enum { READ_END, WRITE_END };
enum { FLUSHED, UNFLUSHED };

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

typedef struct {
    int sizes[2][2];
    char *bufs[2][2];
} BUFS;

typedef struct{
    TestProgram *current_program;
    TestSuite   *current_suite;
    Test        *current_test;
    
    BUFS        *bufs;
    
    int         pipes[2][2][2];

    int         *indent_level;
    
    int         flushed;
    int         in_fork;
    int         STATUS;
    int         EXIT_CODE;
    int         SIGNAL_CODE;    
    
} JankUnitContext;

void print_with_indent  (const char *fmt, ...);
void vprint_with_indent (const char *fmt, va_list args);
void print_result       (const char *color, const char *fmt, ...);
void vprint_result      (const char *color, const char *fmt, va_list args);

void configure_ctx_pre_fork             (void);
void configure_ctx_post_fork            (void);
void dup2_usr_pipes                     (void);
void flush_usr_pipes_and_dup2_sys_pipes (void);
void exit_with_flush                    (void);
void read_pipes_in_parent               (void);
void read_pipes_to_bufs                 (void);

#endif
