#include "jankunit.h"
#include "jankunit.internal.h"

#include "eprintf.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#define COLOR_GREEN "\x1b[32m"
#define COLOR_RED "\x1b[31m"
#define COLOR_RESET "\x1b[0m"

#define BUFSIZE 1024

void init_ctx(void)
{
    JankUnitContext *ctx = (JankUnitContext *) malloc(sizeof(JankUnitContext));

    // uninitialized for this POC test case
    ctx->current_program = NULL;
    ctx->current_suite   = NULL;
    ctx->current_test = NULL;

    //bufsuninitialized
    ctx->bufs = NULL;
    
    // no op for ctx->pipes

    // indent level init
    ctx->indent_level = (int *) emalloc(sizeof(int));
    *(ctx->indent_level) = 0;

    // other
    ctx->flushed = -1;
    ctx->in_fork = 0;
    ctx->STATUS = -1;
    ctx->EXIT_CODE = -1;
    ctx->SIGNAL_CODE = -1;

    GLOBAL_CTX = ctx;
    
}

void start_test_program(const char *name_fmt, ...) 
{
    TestProgram *current_program;
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
    GLOBAL_CTX->current_program = current_program;
    (*GLOBAL_CTX->indent_level)++;
}

void end_test_program() 
{
    TestProgram *current_program;

    (*GLOBAL_CTX->indent_level)--;

    current_program = GLOBAL_CTX->current_program;
    if (current_program->failed == 0) {
        print_result(COLOR_GREEN, "Total: %d, %d passed %d failed\n", 
          current_program->total, current_program->passed, current_program->failed);
    } else {
        print_result(COLOR_RED, "Total: %d, %d passed %d failed\n", 
          current_program->total, current_program->passed, current_program->failed);
    }

    free(current_program->program_name);
    free(current_program);
    GLOBAL_CTX->current_program = NULL;
}

void start_test_suite(const char *name_fmt, ...) 
{
    TestSuite *current_suite;
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
    GLOBAL_CTX->current_suite = current_suite;
    (*GLOBAL_CTX->indent_level)++;
}

void end_test_suite() 
{
    TestSuite *current_suite;
    int is_suite_failed;

    (*GLOBAL_CTX->indent_level)--;
    is_suite_failed = 0;

    current_suite = GLOBAL_CTX->current_suite;
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
    GLOBAL_CTX->current_suite = NULL;

    // propagate suit failure or success to program layer above
    if (is_suite_failed)
        FAIL();
    else
        PASS();
}

void start_test(const char *name_fmt, ...) 
{
    Test *current_test;
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

    GLOBAL_CTX->current_test = current_test;
    (*GLOBAL_CTX->indent_level)++;
}

void end_test() 
{
    Test *current_test;
    int is_test_failed;

    (*GLOBAL_CTX->indent_level)--;
    is_test_failed = 0;

    current_test = GLOBAL_CTX->current_test;
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
    GLOBAL_CTX->current_test = NULL;

    // propagate test success or failure to suite layer above
    if (is_test_failed)
        FAIL();
    else
        PASS();
}

// Helper function for printing with indentation
void print_with_indent(const char *fmt, ...) 
{
    va_list args;
    va_start(args, fmt);

    for (int i = 0; i < *GLOBAL_CTX->indent_level; i++)
        printf("    ");  // Four spaces per level

    vprintf(fmt, args);
    va_end(args);
}

void vprint_with_indent(const char *fmt, va_list args)
{
    for (int i = 0; i < *GLOBAL_CTX->indent_level; i++)
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

// This will be called right before a FORK block
void configure_ctx_pre_fork()
{
    TestProgram  *tp, *sh_tp;
    TestSuite    *ts, *sh_ts;
    Test         *t, *sh_t;
    BUFS         *bufs;
    int          *indent_level, *sh_indent_level;

    sh_tp = NULL;
    sh_ts = NULL;
    sh_t = NULL;
    sh_indent_level = NULL;
    
    /*  
        The following values, if they exist, 
        should be made to use shared memory
        before a fork() call:
            TestProgram *current_program
            TestSuite   *current_suite
            Test        *current_test
            int         *indent_level

    */
    if ((tp = GLOBAL_CTX->current_program)) {
        sh_tp = (TestProgram *) eshmalloc(sizeof(TestProgram));
        sh_tp->total         = tp->total;
        sh_tp->passed        = tp->passed;
        sh_tp->failed        = tp->failed;
        sh_tp->failed_assert = tp->failed_assert;
        if (tp->program_name) {
            eshasprintf(&(sh_tp->program_name), tp->program_name);
            free(tp->program_name);
        } else {
            sh_tp->program_name = NULL;
        }
        free(tp);
    }
    if ((ts = GLOBAL_CTX->current_suite)) {
        sh_ts = (TestSuite *) eshmalloc(sizeof(TestSuite));
        sh_ts->total         = ts->total;
        sh_ts->passed        = ts->passed;
        sh_ts->failed        = ts->failed;
        sh_ts->failed_assert = ts->failed_assert;
        if (ts->suite_name) {
            eshasprintf(&(sh_ts->suite_name), ts->suite_name);
            free(ts->suite_name);
        } else {
            sh_ts->suite_name = NULL;
        }
        free(ts);
    }
    if ((t = GLOBAL_CTX->current_test)) {
        sh_t = (Test *) eshmalloc(sizeof(Test));
        sh_t->total         = t->total;
        sh_t->passed        = t->passed;
        sh_t->failed        = t->failed;
        sh_t->failed_assert = t->failed_assert;
        if (t->test_name) {
            eshasprintf(&(sh_t->test_name), t->test_name);
            free(t->test_name);
        } else {
            sh_t->test_name = NULL;
        }
        free(t);
    }
    if ((indent_level = GLOBAL_CTX->indent_level)) {
        sh_indent_level = (int *) eshmalloc(sizeof(int));
        *sh_indent_level = *indent_level;
        free(indent_level);
    } else {
        sh_indent_level = (int *) eshmalloc(sizeof(int));
        *sh_indent_level = 0;
    }
    /*
        BUFS will be assumed NULL and 
        will be initialized in regular mem,
        forked process need not access BUFS
        of parent.
        Parent only ever needs initialized BUFS
        to copy pipes in from eventual children
    */
    bufs = (BUFS *) emalloc(sizeof(BUFS));
    for (int i = USR; i <= SYS; i++)
        for (int j = OUT; j <= ERR; j++) {
            bufs->bufs[i][j] = NULL;
            bufs->sizes[i][j] = 0;
        }


    // init pipes of the GLOBAL CTX
    for (int i = USR; i <= SYS; i++)
        for (int j = OUT; j <= ERR; j++)
            if (pipe(GLOBAL_CTX->pipes[i][j]))
                eprintf("failed pipe\n");


    GLOBAL_CTX->current_program = sh_tp;
    GLOBAL_CTX->current_suite = sh_ts;
    GLOBAL_CTX->current_test = sh_t;

    GLOBAL_CTX->indent_level = sh_indent_level;

    GLOBAL_CTX->bufs = bufs;

    GLOBAL_CTX->flushed = 0;
    GLOBAL_CTX->in_fork = 1;
}

// This will be called outside of the FORK block
void configure_ctx_post_fork()
{
    TestProgram  *tp, *sh_tp;
    TestSuite    *ts, *sh_ts;
    Test         *t, *sh_t;
    BUFS         *bufs;
    int          *indent_level, *sh_indent_level;

    read_pipes_in_parent();
    wait(&(GLOBAL_CTX->STATUS));
    
    /*  
        The following values, if they exist, 
        should be made to use shared memory
        before a fork() call:
            TestProgram *current_program
            TestSuite   *current_suite
            Test        *current_test
            int         *indent_level

    */
    if ((sh_tp = GLOBAL_CTX->current_program)) {
        tp = (TestProgram *) emalloc(sizeof(TestProgram));
        tp->total         = sh_tp->total;
        tp->passed        = sh_tp->passed;
        tp->failed        = sh_tp->failed;
        tp->failed_assert = sh_tp->failed_assert;
        if (sh_tp->program_name) {
            easprintf(&(tp->program_name), sh_tp->program_name);
            shfree(tp->program_name);
        } else {
            tp->program_name = NULL;
        }
        shfree(sh_tp);
    }
    if ((sh_ts = GLOBAL_CTX->current_suite)) {
        ts = (TestSuite *) emalloc(sizeof(TestSuite));
        ts->total         = sh_ts->total;
        ts->passed        = sh_ts->passed;
        ts->failed        = sh_ts->failed;
        ts->failed_assert = sh_ts->failed_assert;
        if (sh_ts->suite_name) {
            easprintf(&(ts->suite_name), sh_ts->suite_name);
            shfree(sh_ts->suite_name);
        } else {
            ts->suite_name = NULL;
        }
        shfree(sh_ts);
    }
    if ((sh_t = GLOBAL_CTX->current_test)) {
        t = (Test *) emalloc(sizeof(Test));
        t->total         = sh_t->total;
        t->passed        = sh_t->passed;
        t->failed        = sh_t->failed;
        t->failed_assert = sh_t->failed_assert;
        if (sh_t->test_name) {
            easprintf(&(t->test_name), sh_t->test_name);
            shfree(sh_t->test_name);
        } else {
            t->test_name = NULL;
        }
        shfree(sh_t);
    }
    if ((sh_indent_level = GLOBAL_CTX->indent_level)) {
        indent_level = (int *) emalloc(sizeof(int));
        *indent_level = *sh_indent_level;
        shfree(sh_indent_level);
    } else {
        indent_level = (int *) emalloc(sizeof(int));
        *indent_level = 0;
    }
    /*
        During cleanup, sys buffers will be printed,
        If sys errors exist, program will exit.
        After sys out and err printing,
        sys bufs will be freed.
        USR out and ERR will stay put for use in
        assertion and expectation macros
    */

    if (GLOBAL_CTX->bufs->sizes[SYS][OUT] && GLOBAL_CTX->bufs->bufs[SYS][OUT]) {
        printf(GLOBAL_CTX->bufs->bufs[SYS][OUT]);
        fflush(stdout);
        free(GLOBAL_CTX->bufs->bufs[SYS][OUT]);
    }

    if (GLOBAL_CTX->bufs->sizes[SYS][ERR] && GLOBAL_CTX->bufs->bufs[SYS][ERR]) {
        fprintf(stderr, GLOBAL_CTX->bufs->bufs[SYS][ERR]);
        exit(EXIT_FAILURE);
    }

    // init pipes of the GLOBAL CTX

    GLOBAL_CTX->current_program = tp;
    GLOBAL_CTX->current_suite = ts;
    GLOBAL_CTX->current_test = t;

    GLOBAL_CTX->indent_level = indent_level;

    GLOBAL_CTX->flushed = -1;
    GLOBAL_CTX->in_fork = 0;

    GLOBAL_CTX->EXIT_CODE = WIFEXITED(GLOBAL_CTX->STATUS) ? WEXITSTATUS(GLOBAL_CTX->STATUS) : -1;
    GLOBAL_CTX->SIGNAL_CODE = WIFSIGNALED(GLOBAL_CTX->STATUS) ? WTERMSIG(GLOBAL_CTX->STATUS) : -1;

}

void dup2_usr_pipes()
{
    for (int i = USR; i <= SYS; i++)
        for (int j = OUT; j <= ERR; j++)
            close(GLOBAL_CTX->pipes[i][j][READ_END]);

    dup2(GLOBAL_CTX->pipes[USR][OUT][WRITE_END], STDOUT_FILENO);
    dup2(GLOBAL_CTX->pipes[USR][ERR][WRITE_END], STDERR_FILENO);
}

void flush_usr_pipes_and_dup2_sys_pipes()
{
    fflush(stdout);
    
    close(GLOBAL_CTX->pipes[USR][OUT][WRITE_END]);
    close(GLOBAL_CTX->pipes[USR][ERR][WRITE_END]);

    dup2(GLOBAL_CTX->pipes[SYS][OUT][WRITE_END], STDOUT_FILENO);
    dup2(GLOBAL_CTX->pipes[SYS][ERR][WRITE_END], STDERR_FILENO);
}

void exit_with_flush()
{
    fflush(stdout);
    close(GLOBAL_CTX->pipes[SYS][OUT][WRITE_END]);
    close(GLOBAL_CTX->pipes[SYS][ERR][WRITE_END]);
    exit(EXIT_SUCCESS);
}

void read_pipes_in_parent()
{
    // close write ends of all pipes in parent
    for (int i = USR; i <= SYS; i++)
        for (int j = OUT; j <= ERR; j++)
            close(GLOBAL_CTX->pipes[i][j][WRITE_END]);

    read_pipes_to_bufs();

    // after reading, close read ends of all pipes in parent
    for (int i = USR; i <= SYS; i++)
        for (int j = OUT; j <= ERR; j++)
            close(GLOBAL_CTX->pipes[i][j][READ_END]);
}

void read_pipes_to_bufs()
{
    int ntot[2][2]  = { { 0, 0 }, { 0, 0 } };
    int nread[2][2] = { { 0, 0 }, { 0, 0 } };
    
    char temp_bufs[2][2][BUFSIZE];

    while ((nread[USR][OUT] = read( GLOBAL_CTX->pipes[USR][OUT][READ_END], 
                                            temp_bufs[USR][OUT], BUFSIZE)) > 0
        || (nread[USR][ERR] = read( GLOBAL_CTX->pipes[USR][ERR][READ_END], 
                                            temp_bufs[USR][ERR], BUFSIZE)) > 0
        || (nread[SYS][OUT] = read( GLOBAL_CTX->pipes[SYS][OUT][READ_END], 
                                            temp_bufs[SYS][OUT], BUFSIZE)) > 0
        || (nread[SYS][ERR] = read( GLOBAL_CTX->pipes[SYS][ERR][READ_END], 
                                            temp_bufs[SYS][ERR], BUFSIZE)) > 0 
    ) {
        for (int i = USR; i <= SYS; i++)
            for (int j = OUT; j <= ERR; j++) {
                char **cur_buf = &(GLOBAL_CTX->bufs->bufs[i][j]);
                int  *cur_size = &(GLOBAL_CTX->bufs->sizes[i][j]);
        
                if (*cur_buf == NULL) {
                    *cur_buf = (char *) emalloc(sizeof(char) * BUFSIZE);
                    *cur_size = BUFSIZE;
                } else if ((nread[i][j] + ntot[i][j]) > *cur_size) {
                    *cur_buf = (char *) erealloc(cur_buf, (*cur_size * 2));    
                    *cur_size *= 2;
                }
                memcpy((*cur_buf + ntot[i][j]), temp_bufs[i][j], nread[i][j]);
                ntot[i][j] += nread[i][j];
            }    
    }
    // NULL terminate bufs that exist
    for (int i = USR; i <= SYS; i++)
        for (int j = OUT; i <= ERR; i++)
            if (GLOBAL_CTX->bufs->bufs[i][j] != NULL)
                GLOBAL_CTX->bufs->bufs[i][j][ntot[i][j]] = '\0';

}