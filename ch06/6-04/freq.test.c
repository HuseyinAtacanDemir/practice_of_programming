#include "freq.internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
#include <ctype.h>

#include "eprintf.h"

// TEST RUNNERS
  // set_opt_bit
    void  test_set_opt_bit_all_chars            (int *, int *, int *);
    void  test_set_opt_bit_boundaries           (int *, int *, int *);
    void  test_set_opt_bit_nonzero_optstates    (int *, int *, int *);

  // parse_opts  
    void test_parse_opts_single_short_opts      (int *, int *, int *);

// TEST_LOGIC
  int test_set_opt_bit (int opt, unsigned old_optstate, 
                        unsigned exp_optstate, char *exp_msg);
  int test_parse_opts  (int argc, char **argv, unsigned exp_optstate, 
                        int exp_size, int exp_optind, char *exp_msg);


// HELPERS
  void create_pipe      (int pipefd[]);
  void read_pipe_to_buf (char **buf, int pipefd[]);
  char **create_argv    (int argc, ...);

int main(void)
{
    int total, pass, fail;
    
    total = pass = fail = 0;
    printf("FREQ UNIT TEST SUITE\n");
   
    printf("\nSET_OPT_BIT:\n"); 
    test_set_opt_bit_all_chars(&total, &pass, &fail);
    test_set_opt_bit_boundaries(&total, &pass, &fail);
    test_set_opt_bit_nonzero_optstates(&total, &pass, &fail);

    printf("\nPARSE_OPTS:\n"); 
    test_parse_opts_single_short_opts(&total, &pass, &fail);

    printf("\nTotal: %d, Passed: %d, Failed: %d\n", total, pass, fail);
    return 0;
}

void test_set_opt_bit_all_chars(int *total, int *pass, int *fail)
{
    int local_total, local_pass, local_fail, opt;
    unsigned exp_optstate, opt_state;
    char exp_msg[128];

    local_total = local_pass = local_fail = 0;
    printf("\twhole char set individually\n");
    for (opt = 0; opt <= UCHAR_MAX; opt++) {
        opt_state = exp_optstate = 0x0;
        printf("\t\tShort Opt: %c \\x%X: ", (isprint(opt) ? opt : '?'), opt);
        fflush(stdout);
        switch(opt) {
            case 'h':
                exp_optstate |= 1 << HELP;
                break;
            case 'a':
                exp_optstate |= 1 << AGGR;
                break;
            case 'D':
                exp_optstate |= 1 << DELIM;
                break;
            case 'R':
                exp_optstate |= 1 << RAW;
                break;
            case 's':
                exp_optstate |= 1 << SORT;
                break;
            case 'i':
                exp_optstate |= 1 << INT;
                break;
            case 'd':
                exp_optstate |= 1 << DOUBLE;
                break;
            case 'f':
                exp_optstate |= 1 << FLOAT;
                break;
            case 'l': 
                exp_optstate |= 1 << LONG;
                break;
            case 'S':
                exp_optstate |= 1 << STRUCT;
                break;
            default:
                sprintf(exp_msg, "freq_test: invalid option -%c\n", opt);
        }
        if(test_set_opt_bit(opt, opt_state, exp_optstate, exp_msg))
            local_pass++;
        else
            local_fail++;
        local_total++;
    }

    printf("\t\tTotal: %d, Passed: %d, Failed: %d\n", local_total, local_pass, local_fail);
    *total += local_total;
    *pass += local_pass;
    *fail += local_fail;
}

void test_set_opt_bit_boundaries(int *total, int *pass, int *fail)
{
    int i, local_total, local_pass, local_fail;
    char exp_msg[128];
    unsigned exp_optstate, opt_state;

    printf("\topt boundaries\n"); 

    local_total = local_pass = local_fail = 0;
    exp_optstate = opt_state = 0x0;

    int test_cases[] = { INT_MAX, INT_MIN, UCHAR_MAX+1, CHAR_MIN-1 };

    for (i = 0; i < (sizeof(test_cases)/sizeof(int)); i++) {
        printf("\t\tOpt: %d: ", test_cases[i]);
        fflush(stdout);
        sprintf(exp_msg, "freq_test: invalid option format: %d\n", test_cases[i]);
        if (test_set_opt_bit(test_cases[i], opt_state, exp_optstate, exp_msg))
            local_pass++;
        else
            local_fail++;
        local_total++;
    }
    printf("\t\tTotal: %d, Passed: %d, Failed: %d\n", local_total, local_pass, local_fail);
    *total += local_total;
    *pass += local_pass;
    *fail += local_fail;
}

void test_set_opt_bit_nonzero_optstates(int *total, int *pass, int *fail)
{
    int i, local_total, local_pass, local_fail;

    struct TESTCASE {
        unsigned  old_optstate;
        unsigned  new_optstate;
        int       opt;
        char      exp_msg[128];
    };

    printf("\tnonezero optstates\n"); 

    local_total = local_pass = local_fail = 0;
    struct TESTCASE cases[] = {
        { 0x04, (0x04 | (1 << HELP)),   'h', ""},
        { 0x05, (0x05 | (1 << AGGR)),   'a', ""},
        { 0x06, (0x06 | (1 << DELIM)),  'D', ""},
        { 0x07, (0x07 | (1 << RAW)),    'R', ""},
        { 0x08, (0x08 | (1 << SORT)),   's', ""},
        { 0x09, (0x09 | (1 << INT)),    'i', ""},
        { 0x0A, (0x0A | (1 << DOUBLE)), 'd', ""},
        { 0x0B, (0x0B | (1 << FLOAT)),  'f', ""},
        { 0x0C, (0x0C | (1 << LONG)),   'l', ""},
        { 0x0D, (0x0D | (1 << STRUCT)), 'S', ""},
        { 0x0E, 0x0E,                   'x', "freq_test: invalid option -x\n"}
    };

    for (i = 0; i < (sizeof(cases)/sizeof(cases[0])); i++) {
        printf("\t\tOld_optstate: %u, Expected_optstate: %u, Opt: %d: ", 
                cases[i].old_optstate, cases[i].new_optstate, cases[i].opt);
        fflush(stdout);
        if (test_set_opt_bit(cases[i].opt, cases[i].old_optstate, 
                        cases[i].new_optstate, cases[i].exp_msg))
            local_pass++;
        else
            local_fail++;
        local_total++;
    }

    printf("\t\tTotal: %d, Passed: %d, Failed: %d\n", local_total, local_pass, local_fail);
    *total += local_total;
    *pass += local_pass;
    *fail += local_fail;
}

int test_set_opt_bit(int opt, unsigned old_optstate, unsigned exp_optstate, char *exp_msg)
{
    pid_t pid;
    int status, pipefd[2];
    unsigned opt_state;
    char *buf = NULL;

    assert(pipe(pipefd) == 0);

    pid = fork();
    assert(pid >= 0);

    if (pid == 0) { // child process, redirect stderr to pipe
        create_pipe(pipefd);

        opt_state = old_optstate;
        set_opt_bit(&opt_state, opt);

        assert(opt_state == exp_optstate);
        exit(EXIT_SUCCESS);
    } else { // parent
        close(pipefd[1]); // close write end of the pipe

        wait(&status);
        read_pipe_to_buf(&buf, pipefd);
        if (WIFSIGNALED(status) && WTERMSIG(status) == SIGABRT) {
            printf("Failed: %s\n", buf);
            return 0;
        } else if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE) {
            if (strcmp(buf, exp_msg) != 0) {
                printf("Failed: Expected: %s Actual: %s\n", exp_msg, buf);
                return 0;
            }
            printf("Pass\n");
            return 1;
        } else {
            printf("Pass\n");
            return 1;
        }
    }
}

void test_parse_opts_single_short_opts(int *total, int *pass, int *fail)
{
    int i, local_total, local_pass, local_fail;
    char usage_info_str[1024];
     
    struct TESTCASE {
        int       argc;
        int       exp_size;
        int       exp_optind;
        unsigned  exp_optstate;
        char      *exp_msg;
        char      **argv;
    };

    
    printf("\tsingle short opts\n"); 

    sprintf(usage_info_str, "freq_test: %s\n", USAGE_INFO_STR);

    struct TESTCASE cases[] = {
        { 1, 0, 0, 0, "",              create_argv(1, "./freq") },
        { 2, 0, 2, 1, usage_info_str,  create_argv(2, "./freq", "-h") }
    };

    local_total = local_pass = local_fail = 0;
    for (i = 0; i < (sizeof(cases)/sizeof(cases[0])); i++) {
        printf("\t\targc: %d, exp_optstate: %u, exp_optind: %d: ", 
              cases[i].argc, cases[i].exp_optstate, cases[i].exp_optind);
        fflush(stdout);
        if(test_parse_opts(cases[i].argc, cases[i].argv, cases[i].exp_optstate,
                           cases[i].exp_size, cases[i].exp_optind, cases[i].exp_msg))
            local_pass++;
        else
            local_fail++;
        local_total++;
    }


    printf("\t\tTotal: %d, Passed: %d, Failed: %d\n", local_total, local_pass, local_fail);
    *total += local_total;
    *pass += local_pass;
    *fail += local_fail;
}

int test_parse_opts(int argc, char **argv, unsigned exp_optstate, 
                    int exp_size, int exp_optind, char *exp_msg)
{
    pid_t pid;
    int status, size, pipefd[2];
    unsigned opt_state;
    char *delim, *buf;

    buf = NULL;
    assert(pipe(pipefd) == 0);

    pid = fork();
    assert(pid >= 0);

    if (pid == 0) {
        create_pipe(pipefd);
        delim = NULL;
        size = 0;

        opt_state = parse_opts(argc, argv, &delim, &size);

        assert(opt_state == exp_optstate);
        assert(size == exp_size);
        assert(optind == exp_optind);

        exit(EXIT_SUCCESS);
    } else {
        close(pipefd[1]);
        wait(&status);
        read_pipe_to_buf(&buf, pipefd);

        if (WIFSIGNALED(status) && WTERMSIG(status) == SIGABRT) {
            printf("Failed: %s\n", buf);
            return 0;
        } else if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE) {
            if (strcmp(buf, exp_msg) != 0) {
                printf("Failed: Expected: %s Actual: %s\n", exp_msg, buf);
                return 0;
            }
            printf("Pass\n");
            return 1;
        } else if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS) {
            if (strcmp(buf, exp_msg) != 0) {
                printf("Failed: Expected: %s Actual: %s\n", exp_msg, buf);
                return 0;
            }
            printf("Pass\n");
            return 1;
        } else {
            printf("Pass\n");
            return 1;
        }
    }
}

void create_pipe(int pipefd[])
{
    close(pipefd[0]);               // Close read end of the pipe
    dup2(pipefd[1], STDERR_FILENO); // Redirect stderr to pipe write end
    close(pipefd[1]);               // Close write end after dup2
}

void read_pipe_to_buf(char **buf, int pipefd[])
{
    int nbytes, total_read, buf_size;
    char temp_buf[256];  // Temporary buffer for reading chunks

    total_read  = 0;
    buf_size    = 256;
    *buf        = emalloc(buf_size);

    // Read pipe in chunks, dynamically resize buffer if needed
    while ((nbytes = read(pipefd[0], temp_buf, sizeof(temp_buf))) > 0) {
        if ( (total_read + nbytes) >= buf_size) {
            buf_size *= 2;
            *buf = erealloc(*buf, buf_size);
        }
        memcpy( (*buf+total_read), temp_buf, nbytes);
        total_read += nbytes;
    }

    (*buf)[total_read] = '\0'; // Null-terminate the buffer
    close(pipefd[0]);
}

char **create_argv(int argc, ...)
{
    va_list args;
    va_start(args, argc);

    char **argv = emalloc((argc + 1) * sizeof(char *));
    for (int i = 0; i < argc; i++) {
        argv[i] = va_arg(args, char *);
    }
    argv[argc] = NULL;  // Null-terminate the argv array

    va_end(args);
    return argv;
}

