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
    void test_parse_opts_single_short_opts       (int *, int *, int *);
    void test_parse_opts_short_combined_single   (int *, int *, int *);
    void test_parse_opts_short_combined_concat   (int *, int *, int *);

// TEST_LOGIC
  int test_set_opt_bit (int opt, unsigned old_optstate, 
                        unsigned exp_optstate, char *exp_msg);
  int test_parse_opts  (int argc, char **argv, unsigned exp_optstate, 
                        int exp_size, int exp_optind, char *exp_msg);

// HELPERS
  void create_pipe      (int pipefd[]);
  void read_pipe_to_buf (char **buf, int pipefd[]);
  char **create_argv    (int argc, ...);
  char *concat_str_arr  (char **arr, const char *delim);

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
    test_parse_opts_short_combined_single(&total, &pass, &fail);
    test_parse_opts_short_combined_concat(&total, &pass, &fail);

    printf("\nTotal: %d, Passed: %d, Failed: %d\n", total, pass, fail);
    return 0;
}

void test_set_opt_bit_all_chars(int *total, int *pass, int *fail)
{
    int local_total, local_pass, local_fail, opt;
    unsigned exp_optstate, opt_state;
    char exp_msg[128];

    local_total = local_pass = local_fail = 0;
    printf("\n\twhole char set individually\n");
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

    printf("\t\tTotal: %d, Passed: %d, Failed: %d\n", 
            local_total, local_pass, local_fail);
    *total += local_total;
    *pass += local_pass;
    *fail += local_fail;
}

void test_set_opt_bit_boundaries(int *total, int *pass, int *fail)
{
    int i, local_total, local_pass, local_fail;
    char exp_msg[128];
    unsigned exp_optstate, opt_state;

    printf("\n\topt boundaries\n"); 

    local_total = local_pass = local_fail = 0;
    exp_optstate = opt_state = 0x0;

    int test_cases[] = { INT_MAX, INT_MIN, UCHAR_MAX+1, CHAR_MIN-1 };

    for (i = 0; i < (sizeof(test_cases)/sizeof(int)); i++) {
        printf("\t\tOpt: %d: ", test_cases[i]);
        fflush(stdout);
        sprintf(exp_msg, "freq_test: invalid option format: %d\n", 
                test_cases[i]);
        if (test_set_opt_bit(test_cases[i], opt_state, exp_optstate, exp_msg))
            local_pass++;
        else
            local_fail++;
        local_total++;
    }
    printf("\t\tTotal: %d, Passed: %d, Failed: %d\n", 
            local_total, local_pass, local_fail);
    *total += local_total;
    *pass += local_pass;
    *fail += local_fail;
}

void test_set_opt_bit_nonzero_optstates(int *total, int *pass, int *fail)
{
    int i, local_total, local_pass, local_fail;
    struct TestCase {
        unsigned  old_optstate;
        unsigned  new_optstate;
        int       opt;
        char      exp_msg[128];
    };

    printf("\n\tnonzero optstates\n"); 

    local_total = local_pass = local_fail = 0;
    struct TestCase cases[] = {
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

    printf("\t\tTotal: %d, Passed: %d, Failed: %d\n", 
            local_total, local_pass, local_fail);
    *total += local_total;
    *pass += local_pass;
    *fail += local_fail;
}

int test_set_opt_bit(int opt, unsigned old_optstate, 
                      unsigned exp_optstate, char *exp_msg)
{
    pid_t pid;
    int status, pipefd[2], pass;
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
        if (!WIFSIGNALED(status) && !WIFEXITED(status)) {
            printf("Pass\n");
            pass = 1;
        } else if (WIFSIGNALED(status) && WTERMSIG(status) == SIGABRT) {
            printf("Failed: %s\n", buf);
            pass = 0;
        } else if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE 
                   && strcmp(buf, exp_msg) != 0) {
            printf("Failed: Expected: %s Actual: %s\n", exp_msg, buf);
            pass = 0;
        } else {
            printf("Pass\n");
            pass = 1;
        }
        if (buf)
            free(buf);
        return pass;
    }
}

//TODO: fix the freeing logic, you are estrdup'ing everywhere!!
void test_parse_opts_single_short_opts(int *total, int *pass, int *fail)
{
    int i, local_total, local_pass, local_fail;
    char exp_msg[1024], opt[4];
     
    struct TestCase {
        int       argc;
        int       exp_size;
        int       exp_optind; // GNU getopt intializes optind to 1
        unsigned  exp_optstate;
        char      *exp_msg;
        char      **argv;
    } cases[UCHAR_MAX+1];
    
    printf("\n\tsingle short opts\n"); 

    for (i = 0; i <= UCHAR_MAX; i++) {
        switch(i) {
            case 0:
                cases[i] = (struct TestCase){ 1, 0, 1, 0, "", 
                                             create_argv(1, "./freq") };
                break;
            case 'a':
                cases[i] = (struct TestCase){ 2, 0, 2, 1 << AGGR, "", 
                                              create_argv(2, "./freq", "-a") };
                break;
            case 'R':
                cases[i] = (struct TestCase){ 2, 0, 2, 1 << RAW, "", 
                                              create_argv(2, "./freq", "-R") };
                break;
            case 's':
                cases[i] = (struct TestCase){ 2, 0, 2, 1 << SORT, "", 
                                              create_argv(2, "./freq", "-s") };
                break;
            case 'i':
                cases[i] = (struct TestCase){ 2, 0, 2, 1 << INT, "", 
                                              create_argv(2, "./freq", "-i") };
                break;
            case 'd':
                cases[i] = (struct TestCase){ 2, 0, 2, 1 << DOUBLE, "", 
                                              create_argv(2, "./freq", "-d") };
                break;
            case 'f':
                cases[i] = (struct TestCase){ 2, 0, 2, 1 << FLOAT, "", 
                                              create_argv(2, "./freq", "-f") };
                break;
            case 'l':
                cases[i] = (struct TestCase){ 2, 0, 2, 1 << LONG, "", 
                                              create_argv(2, "./freq", "-l") };
                break;
            case 'h':
                sprintf(exp_msg, "freq_test: %s\n", UsageInfoStr);
                cases[i] = (struct TestCase){ 2, 0, 2, 1 << HELP, 
                            estrdup(exp_msg), create_argv(2, "./freq", "-h")};
                break;
            case 'D':
                sprintf(exp_msg, 
                        "freq_test: option -%c requires an argument\n", i);
                cases[i] = (struct TestCase){ 2, 0, 2, 1 << DELIM,  
                            estrdup(exp_msg), create_argv(2, "./freq", "-D") };
                break;
            case 'S':
                sprintf(exp_msg, 
                        "freq_test: option -%c requires an argument\n", i);
                cases[i] = (struct TestCase){ 2, 0, 2, 1 << STRUCT, 
                            estrdup(exp_msg), create_argv(2, "./freq", "-S") };
                break;
            case '-':
                cases[i] = (struct TestCase){ 2, 0, 2, 0, "", 
                                              create_argv(2, "./freq", "--")};
                break;
            default:
                sprintf(opt, "-%c", i);
                sprintf(exp_msg, "freq_test: invalid option -%c\n", i);
                cases[i] = (struct TestCase){ 2, 0, 2, 0, 
                    estrdup(exp_msg), create_argv(2, "./freq", estrdup(opt))};
                break;
        }
    }

    local_total = local_pass = local_fail = 0;
    for (i = 0; i < (sizeof(cases)/sizeof(cases[0])); i++) {
        printf("\t\tcmd: \"%s\" argc: %d, exp_optstate: %u, exp_optind: %d: ", 
                concat_str_arr(cases[i].argv, " "),cases[i].argc, 
                cases[i].exp_optstate, cases[i].exp_optind);
        fflush(stdout);
        if(test_parse_opts(cases[i].argc, cases[i].argv, cases[i].exp_optstate,
                     cases[i].exp_size, cases[i].exp_optind, cases[i].exp_msg))
            local_pass++;
        else
            local_fail++;
        local_total++;
    }
    
    for (i = 0; i <= UCHAR_MAX; i++) {
        free(cases[i].argv);
        if (strcmp("", cases[i].exp_msg) != 0)
            free(cases[i].exp_msg);
    }

    printf("\t\tTotal: %d, Passed: %d, Failed: %d\n", 
            local_total, local_pass, local_fail);
    *total += local_total;
    *pass += local_pass;
    *fail += local_fail;
}

void test_parse_opts_short_combined_single(int *total, int *pass, int *fail)
{
    int i, local_total, local_pass, local_fail;
    struct TestCase {
        int       argc;
        int       exp_size;
        int       exp_optind; // GNU getopt intializes optind to 1
        unsigned  exp_optstate;
        char      *exp_msg;
        char      **argv;
    };

    printf("\n\tshort opts combined separate\n"); 

    local_total = local_pass = local_fail = 0;

    struct TestCase cases[] = {
    // Valid combinations
    {3, 0, 3, 0x28, "", 
      create_argv(3, "./freq_test", "-i", "-R")}, 
    
    {4, 0, 4, 0x0C, "", 
      create_argv(4, "./freq_test", "-D", "\"delim\"", "-R")},  
    
    {4, 0, 4, 0x112, "", 
      create_argv(4, "./freq_test", "-s", "-a", "-l")}, 
    
    {3, 0, 3, 0x88, "", 
      create_argv(3, "./freq_test", "-f", "-R")}, 
    
    {4, 10, 4, 0x208, "", 
      create_argv(4, "./freq_test", "-S", "10", "-R")}, 

    // Invalid combinations (mutually exclusive)
    {3, 0, 3, 0x00, "freq_test: Mutually exclusive options (-i -d -f -l -S) "
      "cannot be used together\n", 
      create_argv(3, "./freq_test", "-i", "-d")}, 
    
    {3, 0, 3, 0x00, "freq_test: Mutually exclusive options (-i -d -f -l -S) "
      "cannot be used together\n", 
      create_argv(3, "./freq_test", "-f", "-l")}, 
    
    {5, 0, 6, 0x00, "freq_test: Mutually exclusive options (-i -d -f -l -S) "
      "cannot be used together\n", 
      create_argv(5, "./freq_test", "-i", "-d", "-S", "5")}, 
    
    {4, 0, 4, 0x00, "freq_test: Mutually exclusive options (-i -d -f -l -S) "
      "cannot be used together\n", 
      create_argv(4, "./freq_test", "-S", "5", "-i")}, 
    
    {4, 0, 4, 0x00, "freq_test: Mutually exclusive options (-i -d -f -l -S) "
      "cannot be used together\n", 
      create_argv(4, "./freq_test","-S", "5", "-d")},  

    // Missing required arguments
    {3, 0, 3, 0x00, "freq_test: option -D requires an argument in quotes: "
      "\"arg\"\n", 
      create_argv(3, "./freq_test", "-D", "-R")},  
    
    {3, 0, 3, 0x00, "freq_test: option -S requires an argument\n", 
      create_argv(3, "./freq_test", "-S", "-R")},  

    // Option -S requires -R
    {4, 0, 4, 0x00, "freq_test: option -S SIZE (--struct=SIZE) requires -R "
      "(--raw) option.\n", 
      create_argv(4, "./freq_test", "-S", "10", "-s")},  
    
    {4, 10, 4, 0x208, "", 
      create_argv(4, "./freq_test", "-S", "10", "-R")},  

    // Valid multiple short options
    {7, 0, 7, 0x3E, "", // 0000 0011 1110 
      create_argv(7, "./freq_test", "-i", "-R", "-s", "-a", "-D", "\",\"")},  
    {4, 0, 4, 0x98, "", 
      create_argv(5, "./freq_test", "-f", "-R", "-s")},

    {0, 0, 0, 0, "", NULL}  
    };

    for (i = 0; cases[i].argc != 0; i++) {
        printf("\t\tcmd: \"%s\" argc: %d, exp_optstate: %u, exp_optind: %d: ", 
                concat_str_arr(cases[i].argv, " "), cases[i].argc, 
                cases[i].exp_optstate, cases[i].exp_optind);
        fflush(stdout);
        if (test_parse_opts(cases[i].argc,cases[i].argv,cases[i].exp_optstate,
                     cases[i].exp_size, cases[i].exp_optind, cases[i].exp_msg))
            local_pass++;
        else
            local_fail++;
        local_total++;
    }

    printf("\t\tTotal: %d, Passed: %d, Failed: %d\n", 
            local_total, local_pass, local_fail);
    *total += local_total;
    *pass += local_pass;
    *fail += local_fail;    
}

void test_parse_opts_short_combined_concat(int *total, int *pass, int *fail)
{
    int i, local_total, local_pass, local_fail;
    struct TestCase {
        int       argc;
        int       exp_size;
        int       exp_optind; // GNU getopt intializes optind to 1
        unsigned  exp_optstate;
        char      *exp_msg;
        char      **argv;
    };
    
    printf("\n\tshort opts combined together\n"); 

    local_total = local_pass = local_fail = 0;

    struct TestCase cases[] = {
    // Valid combinations
    {2, 0, 2, 0x28, "", 
      create_argv(2, "./freq_test", "-iR")}, 
    
    {3, 0, 3, 0x0C, "", 
      create_argv(3, "./freq_test", "-RD", "\"delim\"")},  
    
    {2, 0, 2, 0x112, "", 
      create_argv(2, "./freq_test", "-sal")}, 
    
    {2, 0, 2, 0x88, "", 
      create_argv(2, "./freq_test", "-fR")}, 
    
    {3, 10, 3, 0x208, "", 
      create_argv(3, "./freq_test", "-SR", "10")}, 

    // Invalid combinations (mutually exclusive)
    {2, 0, 2, 0x00, "freq_test: Mutually exclusive options (-i -d -f -l -S) "
      "cannot be used together\n", 
      create_argv(2, "./freq_test", "-id")}, 
    
    {2, 0, 2, 0x00, "freq_test: Mutually exclusive options (-i -d -f -l -S) "
      "cannot be used together\n", 
      create_argv(2, "./freq_test", "-fl")}, 
    
    {3, 0, 3, 0x00, "freq_test: Mutually exclusive options (-i -d -f -l -S) "
      "cannot be used together\n", 
      create_argv(3, "./freq_test", "-idS", "5")}, 
    
    {3, 0, 3, 0x00, "freq_test: Mutually exclusive options (-i -d -f -l -S) "
      "cannot be used together\n", 
      create_argv(3, "./freq_test", "-Si", "5")}, 
    
    {3, 0, 3, 0x00, "freq_test: Mutually exclusive options (-i -d -f -l -S) "
      "cannot be used together\n", 
      create_argv(3, "./freq_test","-Sd", "5")},  

    // Missing required arguments
    {2, 0, 2, 0x00, "freq_test: option -D requires an argument in quotes: "
      "\"arg\"\n", 
      create_argv(2, "./freq_test", "-DR")},  
    
    {2, 0, 2, 0x00, "freq_test: option -S requires an argument\n", 
      create_argv(2, "./freq_test", "-SR")},  

    // Option -S requires -R
    {3, 0, 3, 0x00, "freq_test: option -S SIZE (--struct=SIZE) requires -R "
      "(--raw) option.\n", 
      create_argv(3, "./freq_test", "-Ss", "10")},  
    
    {3, 10, 3, 0x208, "", 
      create_argv(3, "./freq_test", "-SR", "10")},  

    // Valid multiple short options
    {3, 0, 3, 0x3E, "", // 0000 0011 1110 
      create_argv(3, "./freq_test", "-iRsaD", "\",\"")},  
    {2, 0, 2, 0x98, "", 
      create_argv(2, "./freq_test", "-fRs")},
    
    {0, 0, 0, 0, "", NULL} 
    };

    for (i = 0; cases[i].argc != 0; i++) {
         printf("\t\tcmd: \"%s\" argc: %d, exp_optstate: %u, exp_optind: %d: ", 
                concat_str_arr(cases[i].argv, " "), cases[i].argc, 
                cases[i].exp_optstate, cases[i].exp_optind);
        fflush(stdout);
        if (test_parse_opts(cases[i].argc,cases[i].argv,cases[i].exp_optstate,
                     cases[i].exp_size, cases[i].exp_optind, cases[i].exp_msg))
            local_pass++;
        else
            local_fail++;
        local_total++;
    }

    printf("\t\tTotal: %d, Passed: %d, Failed: %d\n", 
            local_total, local_pass, local_fail);
    *total += local_total;
    *pass += local_pass;
    *fail += local_fail;  
}

int test_parse_opts(int argc, char **argv, unsigned exp_optstate, 
                    int exp_size, int exp_optind, char *exp_msg)
{
    pid_t pid;
    int status, size, pipefd[2], pass;
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
        if (delim)
            free(delim);
        assert(opt_state == exp_optstate);
        assert(size == exp_size);
        assert(optind == exp_optind);

        exit(EXIT_SUCCESS);
    } else {
        close(pipefd[1]);
        wait(&status);
        read_pipe_to_buf(&buf, pipefd);

        if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE 
                    && strcmp(buf, exp_msg) != 0) {
            printf("Failed: Expected: %s Actual: %s\n", exp_msg, buf);
            pass = 0;
        } else if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS 
                    && strcmp(buf, exp_msg) != 0) {
            printf("Failed: Expected: %s Actual: %s\n", exp_msg, buf);
            pass = 0;
        } else if (WIFSIGNALED(status) && WTERMSIG(status) == SIGABRT) {
            printf("Failed: %s\n", buf);
            pass = 0;
        } else if (WIFSIGNALED(status)) {
            printf("Failed: signal: %d %s\n", WTERMSIG(status), buf);
            pass = 0;
        } else {
            printf("Pass\n");
            pass = 1;
        }
        
        if (buf)
            free(buf);
        return pass;
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
    int i;
    char **argv;
    va_list args;

    va_start(args, argc);

    argv = (char **) emalloc((argc+1) * sizeof(char *));

    for (i = 0; i < argc; i++)
        argv[i] = va_arg(args, char *);
    
    argv[argc] = NULL;

    va_end(args);
    return argv;
}

char *concat_str_arr(char **arr, const char *delim)
{
    int i, delim_len, total_len;
    char *s;

    total_len = 1; // accounting for null terminator
    delim_len = strlen(delim);

    for (i = 0; arr[i] != NULL; i++)
        total_len += strlen(arr[i]) + ((arr[i+1] != NULL) ? delim_len : 0);

    s = emalloc(total_len);

    // Copy strings and delimiters into `s`, strcat copies '\0' as well
    s[0] = '\0';
    for (i = 0; arr[i] != NULL; i++) {
        strcat(s, arr[i]);
        if (arr[i + 1] != NULL)
            strcat(s, delim);
    }

    return s;
}
