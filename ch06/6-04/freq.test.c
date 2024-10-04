#include "freq.internal.h"

#include <stdio.h>
#include <stdlib.h>
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

// TEST_LOGIC
int  test_set_opt_bit (int opt, unsigned old_opt_state, 
                        unsigned expected_opt_state, char *expected_msg);
void test_parse_opts  (int argc, char **argv);

// HELPERS
void create_pipe      (int pipefd[]);
void read_pipe_to_buf (char **buf, int pipefd[]);

int main(void)
{
    int total, pass, fail;
    
    total = pass = fail = 0;
    printf("Freq Test Suite\n");
    
    test_set_opt_bit_all_chars(&total, &pass, &fail);
    test_set_opt_bit_boundaries(&total, &pass, &fail);
    test_set_opt_bit_nonzero_optstates(&total, &pass, &fail);



    printf("\nTotal: %d, Passed: %d, Failed: %d\n", total, pass, fail);
    return 0;
}

void test_set_opt_bit_all_chars(int *total, int *pass, int *fail)
{
    int local_total, local_pass, local_fail, opt;
    unsigned expected_opt_state, opt_state;
    char expected_msg[128];

    local_total = local_pass = local_fail = 0;
    printf("SET_OPT_BIT: whole char set individually\n");
    for (opt = 0; opt <= UCHAR_MAX; opt++) {
        opt_state = expected_opt_state = 0x0;
        printf("\tShort Opt: %c \\x%X: ", (isprint(opt) ? opt : '?'), opt);
        fflush(stdout);
        switch(opt) {
            case 'h':
                expected_opt_state |= 1 << HELP;
                break;
            case 'a':
                expected_opt_state |= 1 << AGGR;
                break;
            case 'D':
                expected_opt_state |= 1 << DELIM;
                break;
            case 'R':
                expected_opt_state |= 1 << RAW;
                break;
            case 's':
                expected_opt_state |= 1 << SORT;
                break;
            case 'i':
                expected_opt_state |= 1 << INT;
                break;
            case 'd':
                expected_opt_state |= 1 << DOUBLE;
                break;
            case 'f':
                expected_opt_state |= 1 << FLOAT;
                break;
            case 'l': 
                expected_opt_state |= 1 << LONG;
                break;
            case 'S':
                expected_opt_state |= 1 << STRUCT;
                break;
            default:
                sprintf(expected_msg, "freq_test: invalid option -%c\n", opt);
        }
        if(test_set_opt_bit(opt, opt_state, expected_opt_state, expected_msg))
            local_pass++;
        else
            local_fail++;
        local_total++;
    }

    printf("\tTotal: %d, Passed: %d, Failed: %d\n", local_total, local_pass, local_fail);
    *total += local_total;
    *pass += local_pass;
    *fail += local_fail;
}

void test_set_opt_bit_boundaries(int *total, int *pass, int *fail)
{
    int i, local_total, local_pass, local_fail;
    char expected_msg[128];
    unsigned expected_opt_state, opt_state;

    printf("SET_OPT_BIT: opt boundaries\n"); 

    local_total = local_pass = local_fail = 0;
    expected_opt_state = opt_state = 0x0;

    int test_cases[] = { INT_MAX, INT_MIN, UCHAR_MAX+1, CHAR_MIN-1 };

    for (i = 0; i < (sizeof(test_cases)/sizeof(int)); i++) {
        printf("\tOpt: %d: ", test_cases[i]);
        fflush(stdout);
        sprintf(expected_msg, "freq_test: invalid option format: %d\n", test_cases[i]);
        if (test_set_opt_bit(test_cases[i], opt_state, expected_opt_state, expected_msg))
            local_pass++;
        else
            local_fail++;
        local_total++;
    }
    printf("\tTotal: %d, Passed: %d, Failed: %d\n", local_total, local_pass, local_fail);
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
        char      expected_msg[128];
    };

    printf("SET_OPT_BIT: nonezero optstates\n"); 

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

    for (i = 0; i < (sizeof(cases)/sizeof(struct TESTCASE)); i++) {
        printf("\tOld_optstate: %u, Expected_optstate: %u, Opt: %d: ", 
                cases[i].old_optstate, cases[i].new_optstate, cases[i].opt);
        fflush(stdout);
        if (test_set_opt_bit(cases[i].opt, cases[i].old_optstate, 
                        cases[i].new_optstate, cases[i].expected_msg))
            local_pass++;
        else
            local_fail++;
        local_total++;
    }

    printf("\tTotal: %d, Passed: %d, Failed: %d\n", local_total, local_pass, local_fail);
    *total += local_total;
    *pass += local_pass;
    *fail += local_fail;
}

int test_set_opt_bit(int opt, unsigned old_opt_state, unsigned expected_opt_state, char *expected_msg)
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

        opt_state = old_opt_state;
        set_opt_bit(&opt_state, opt);

        assert(opt_state == expected_opt_state);
        exit(EXIT_SUCCESS);
    } else { // parent
        close(pipefd[1]); // close write end of the pipe

        wait(&status);
        read_pipe_to_buf(&buf, pipefd);
        if (WIFSIGNALED(status) && WTERMSIG(status) == SIGABRT) {
            printf("Failed: %s\n", buf);
            return 0;
        } else if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE) {
            if (strcmp(buf, expected_msg) != 0) {
                printf("Failed: Expected: %s Actual: %s\n", expected_msg, buf);
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

void test_parse_opts(int argc, char **argv)
{
    return;
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

