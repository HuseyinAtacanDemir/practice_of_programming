#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "eprintf.h"

/*
init pipes using GLOBAL CTX pipe fds
copy whatever GLOBAL_CTX we have to a shared CTX to retrieve the TEST data updates after fork
*/

#define BUFSIZE 1024

#define FORK()                                          \
    for (                                               \
        pid_t _pid_ = (configure_ctx_pre_fork(), -2);   \
        _pid_ == -2;                                    \
        (_pid_ > 0 && configure_ctx_post_fork())) \
        for (                                           \
            _pid_ = fork();                             \
            _pid_ == 0 && (dup2_usr_pipes(), 1);        \
            exit_with_flush())

enum
{
    USR,
    SYS
};
enum
{
    OUT,
    ERR
};
enum
{
    READ_END,
    WRITE_END
};
enum
{
    FLUSHED,
    UNFLUSHED
};
typedef struct {
    char *name;
    int total;
    int passed;
    int failed;
    int failed_assert;
} Test;

typedef struct {
    char *name;
    int total;
    int passed;
    int failed;
    int failed_assert;
} TestSuite;

typedef struct {
    char *name;
    int total;
    int passed;
    int failed;
    int failed_assert;
} TestProgram;

typedef struct
{
    int sizes[2][2];
    char *bufs[2][2];
} BUFS;

typedef struct{
    TestProgram *cur_prog;
    TestSuite   *cur_suite;
    Test        *cur_test;
    
    BUFS        *bufs;
    
    int         pipes[2][2][2];

    int         *indent;
    
    int         flushed;
    int         in_fork;
    int         STATUS;
    int         EXIT_CODE;
    int         SIGNAL_CODE;    
    
} JankUnitContext;


JankUnitContext *GLOBAL_CTX = NULL;
int main(void)
{
    GLOBAL_CTX = init_ctx_with_cur_test();

    pid_t pid = -2;
    configure_ctx_pre_fork();
    pid = fork();
    if (pid == 0) {
        dup2_usr_pipes();
        for (int i = 0; i < 1000; i++) {
            printf("%d ", i);
            fprintf(stderr, "%d ", i);
        }
        // what if user code is aborted/terminated with a signal?
        // we need a signal handler, so taht at least we can close the pipes
        // man 2 sigaction specifies that close is signal safe
        flush_usr_pipes_and_dup2_sys_pipes();
        for (int i = 1000; i < 2000; i++) {
            printf("%d ", i);
            fprintf(stderr, "%d ", i);
        }
        exit_with_flush();
    }
    configure_ctx_post_fork();

    return 0;
}

JankUnitContext *init_ctx_with_cur_test()
{
    JankUnitContext *ctx = (JankUnitContext *) malloc(sizeof(JankUnitContext));

    // uninitialized for this POC test case
    ctx->cur_prog = NULL;
    ctx->cur_suite   = NULL;
       
    // current test init
    ctx->cur_test = (Test *) emalloc(sizeof(Test));
    ctx->cur_test->total = 0;
    ctx->cur_test->passed = 0;
    ctx->cur_test->failed = 0;
    ctx->cur_test->failed_assert = 0;
    easprintf(&(ctx->cur_test->name), "Dummy Test");
    print_with_indent("test: %s\n", ctx->cur_test->name);

    //bufsuninitialized
    ctx->bufs = NULL;
    
    // no op for ctx->pipes

    // indent level init
    ctx->indent = (int *) emalloc(sizeof(int));
    *(ctx->indent) = 0;
    *(ctx->indent)++;

    // other
    ctx->flushed = -1;
    ctx->in_fork = 0;
    ctx->STATUS = -1;
    ctx->EXIT_CODE = -1;
    ctx->SIGNAL_CODE = -1;

    return ctx;
    
}

// This will be called right before a FORK block
void configure_ctx_pre_fork()
{
    TestProgram  *tp, *sh_tp;
    TestSuite    *ts, *sh_ts;
    Test         *t, *sh_t;
    BUFS         *bufs;
    int          *indent, *sh_indent;

    sh_tp = NULL;
    sh_ts = NULL;
    sh_t = NULL;
    sh_indent = NULL;
    
    /*  
        The following values, if they exist, 
        should be made to use shared memory
        before a fork() call:
            TestProgram *cur_prog
            TestSuite   *cur_suite
            Test        *cur_test
            int         *indent

    */
    if ((tp = GLOBAL_CTX->cur_prog)) {
        sh_tp = (TestProgram *) eshmalloc(sizeof(TestProgram));
        sh_tp->total         = tp->total;
        sh_tp->passed        = tp->passed;
        sh_tp->failed        = tp->failed;
        sh_tp->failed_assert = tp->failed_assert;
        if (tp->name) {
            eshasprintf(&(sh_tp->name), tp->name);
            free(tp->name);
        } else {
            sh_tp->name = NULL;
        }
        free(tp);
    }
    if ((ts = GLOBAL_CTX->cur_suite)) {
        sh_ts = (TestSuite *) eshmalloc(sizeof(TestSuite));
        sh_ts->total         = ts->total;
        sh_ts->passed        = ts->passed;
        sh_ts->failed        = ts->failed;
        sh_ts->failed_assert = ts->failed_assert;
        if (ts->name) {
            eshasprintf(&(sh_ts->name), ts->name);
            free(ts->name);
        } else {
            sh_ts->name = NULL;
        }
        free(ts);
    }
    if ((t = GLOBAL_CTX->cur_test)) {
        sh_t = (Test *) eshmalloc(sizeof(Test));
        sh_t->total         = t->total;
        sh_t->passed        = t->passed;
        sh_t->failed        = t->failed;
        sh_t->failed_assert = t->failed_assert;
        if (t->name) {
            eshasprintf(&(sh_t->name), t->name);
            free(t->name);
        } else {
            sh_t->name = NULL;
        }
        free(t);
    }
    if ((indent = GLOBAL_CTX->indent)) {
        sh_indent = (int *) eshmalloc(sizeof(int));
        *sh_indent = *indent;
        free(indent);
    } else {
        sh_indent = (int *) eshmalloc(sizeof(int));
        *sh_indent = 0;
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


    GLOBAL_CTX->cur_prog = sh_tp;
    GLOBAL_CTX->cur_suite = sh_ts;
    GLOBAL_CTX->cur_test = sh_t;

    GLOBAL_CTX->indent = sh_indent;

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
    int          *indent, *sh_indent;

    read_pipes_in_parent();
    wait(&(GLOBAL_CTX->STATUS));
    
    /*  
        The following values, if they exist, 
        should be made to use shared memory
        before a fork() call:
            TestProgram *cur_prog
            TestSuite   *cur_suite
            Test        *cur_test
            int         *indent

    */
    if ((sh_tp = GLOBAL_CTX->cur_prog)) {
        tp = (TestProgram *) emalloc(sizeof(TestProgram));
        tp->total         = sh_tp->total;
        tp->passed        = sh_tp->passed;
        tp->failed        = sh_tp->failed;
        tp->failed_assert = sh_tp->failed_assert;
        if (sh_tp->name) {
            easprintf(&(tp->name), sh_tp->name);
            shfree(tp->name);
        } else {
            tp->name = NULL;
        }
        shfree(sh_tp);
    }
    if ((sh_ts = GLOBAL_CTX->cur_suite)) {
        ts = (TestSuite *) emalloc(sizeof(TestSuite));
        ts->total         = sh_ts->total;
        ts->passed        = sh_ts->passed;
        ts->failed        = sh_ts->failed;
        ts->failed_assert = sh_ts->failed_assert;
        if (sh_ts->name) {
            easprintf(&(ts->name), sh_ts->name);
            shfree(sh_ts->name);
        } else {
            ts->name = NULL;
        }
        shfree(sh_ts);
    }
    if ((sh_t = GLOBAL_CTX->cur_test)) {
        t = (Test *) emalloc(sizeof(Test));
        t->total         = sh_t->total;
        t->passed        = sh_t->passed;
        t->failed        = sh_t->failed;
        t->failed_assert = sh_t->failed_assert;
        if (sh_t->name) {
            easprintf(&(t->name), sh_t->name);
            shfree(sh_t->name);
        } else {
            t->name = NULL;
        }
        shfree(sh_t);
    }
    if ((sh_indent = GLOBAL_CTX->indent)) {
        indent = (int *) emalloc(sizeof(int));
        *indent = *sh_indent;
        shfree(sh_indent);
    } else {
        indent = (int *) emalloc(sizeof(int));
        *indent = 0;
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

    GLOBAL_CTX->cur_prog = tp;
    GLOBAL_CTX->cur_suite = ts;
    GLOBAL_CTX->cur_test = t;

    GLOBAL_CTX->indent = indent;

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

    while (
        (nread[USR][OUT] = read(GLOBAL_CTX->pipes[USR][OUT][READ_END], temp_bufs[USR][OUT], BUFSIZE)) > 0 ||
        (nread[USR][ERR] = read(GLOBAL_CTX->pipes[USR][ERR][READ_END], temp_bufs[USR][ERR], BUFSIZE)) > 0 ||
        (nread[SYS][OUT] = read(GLOBAL_CTX->pipes[SYS][OUT][READ_END], temp_bufs[SYS][OUT], BUFSIZE)) > 0 ||
        (nread[SYS][ERR] = read(GLOBAL_CTX->pipes[SYS][ERR][READ_END], temp_bufs[SYS][ERR], BUFSIZE)) > 0 ) 
    {
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