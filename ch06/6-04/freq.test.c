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
    void test_parse_opts_short_single           (int *, int *, int *);
    void test_parse_opts_short_combined_single  (int *, int *, int *);
    void test_parse_opts_short_combined_concat  (int *, int *, int *);
    void test_parse_opts_long_single            (int *, int *, int *); 
    void test_parse_opts_long_combined_single   (int *, int *, int *); 
    void test_parse_opts_short_long_mixed       (int *, int *, int *); 

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
  char *rus_doll_fmt    (int n, ...);

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
    test_parse_opts_short_single(&total, &pass, &fail);
    test_parse_opts_short_combined_single(&total, &pass, &fail);
    test_parse_opts_short_combined_concat(&total, &pass, &fail);
    test_parse_opts_long_single(&total, &pass, &fail);
    test_parse_opts_long_combined_single(&total, &pass, &fail);
    test_parse_opts_short_long_mixed(&total, &pass, &fail);
    
    printf("\nTotal: %d, Passed: %d, Failed: %d\n", total, pass, fail);
    
    return 0;
}

// region: set_opt_bit
void test_set_opt_bit_all_chars(int *total, int *pass, int *fail)
{
    int ntotal, npass, nfail, opt, n;
    unsigned exp_optstate;
    char *exp_msg, buf[5];

    printf("\n\twhole char set individually\n");

    ntotal = npass = nfail = 0;
    n =  UCHAR_MAX + 1;
    exp_msg = NULL;
    for (opt = 0; opt < n; opt++) {
        printf("\t\tShort Opt: %c \\x%X: ", (isprint(opt) ? opt : '?'), opt);
        fflush(stdout);

        exp_optstate = 0x1;
        switch(opt) {
            case 'S':
                exp_optstate <<= 1;
            case 'l':
                exp_optstate <<= 1;
            case 'f':
                exp_optstate <<= 1;
            case 'd':
                exp_optstate <<= 1; 
            case 'i':
                exp_optstate <<= 1;
            case 's':
                exp_optstate <<= 1; 
            case 'R':
                exp_optstate <<= 1;
            case 'D':
                exp_optstate <<= 1;
            case 'a': 
                exp_optstate <<= 1;
            case 'h':
                break;
            default:
                exp_optstate = 0x0;
                if (opt) {
                  sprintf(buf, "-%c", opt);
                  exp_msg = rus_doll_fmt(3, "freq_test: %s\n", InvOptStr, buf);
                } else {
                  sprintf(buf, "%d", opt);
                  exp_msg = rus_doll_fmt(3, "freq_test: %s\n", InvOptFormat, buf);
                }
        }
         
        if(test_set_opt_bit(opt, 0x0, exp_optstate, exp_msg))
            npass++;
        else
            nfail++;
        ntotal++;

        if (exp_msg != NULL) {
            free(exp_msg);
            exp_msg = NULL;
        }
    }

    printf("\t\tTotal: %d, Passed: %d, Failed: %d\n", 
            ntotal, npass, nfail);
    *total += ntotal;
    *pass += npass;
    *fail += nfail;
}

void test_set_opt_bit_boundaries(int *total, int *pass, int *fail)
{
    int i, ntotal, npass, nfail;
    char exp_msg[128];
    unsigned exp_optstate, opt_state;

    printf("\n\topt boundaries\n"); 

    ntotal = npass = nfail = 0;
    exp_optstate = opt_state = 0x0;

    int test_cases[] = { INT_MAX, INT_MIN, UCHAR_MAX+1, CHAR_MIN-1 };

    for (i = 0; i < (sizeof(test_cases)/sizeof(int)); i++) {
        printf("\t\tOpt: %d: ", test_cases[i]);
        fflush(stdout);
        sprintf(exp_msg, "freq_test: invalid option format: %d\n", 
                test_cases[i]);
        if (test_set_opt_bit(test_cases[i], opt_state, exp_optstate, exp_msg))
            npass++;
        else
            nfail++;
        ntotal++;
    }
    printf("\t\tTotal: %d, Passed: %d, Failed: %d\n", 
            ntotal, npass, nfail);
    *total += ntotal;
    *pass += npass;
    *fail += nfail;
}

void test_set_opt_bit_nonzero_optstates(int *total, int *pass, int *fail)
{
    int i, ntotal, npass, nfail;
    struct TestCase {
        unsigned  old_optstate;
        unsigned  new_optstate;
        int       opt;
        char      *exp_msg;
    };

    printf("\n\tnonzero optstates\n"); 

    ntotal = npass = nfail = 0;
    struct TestCase cases[] = {
        { 0x04, (0x04 | (1 << HELP)),   'h', NULL},
        { 0x05, (0x05 | (1 << AGGR)),   'a', NULL},
        { 0x06, (0x06 | (1 << DELIM)),  'D', NULL},
        { 0x07, (0x07 | (1 << RAW)),    'R', NULL},
        { 0x08, (0x08 | (1 << SORT)),   's', NULL},
        { 0x09, (0x09 | (1 << INT)),    'i', NULL},
        { 0x0A, (0x0A | (1 << DOUBLE)), 'd', NULL},
        { 0x0B, (0x0B | (1 << FLOAT)),  'f', NULL},
        { 0x0C, (0x0C | (1 << LONG)),   'l', NULL},
        { 0x0D, (0x0D | (1 << STRUCT)), 'S', NULL},
        { 0x0E, 0x0E,                   'x', 
                          rus_doll_fmt(3, "freq_test: %s\n", InvOptStr, "-x")},
        { 0, 0, 0, NULL }
    };

    for (i = 0; i < cases[i].old_optstate != 0; i++) {
        printf("\t\tOld_optstate: %u, Expected_optstate: %u, Opt: %d: ", 
                cases[i].old_optstate, cases[i].new_optstate, cases[i].opt);
        fflush(stdout);
        if (test_set_opt_bit(cases[i].opt, cases[i].old_optstate, 
                        cases[i].new_optstate, cases[i].exp_msg))
            npass++;
        else
            nfail++;
        ntotal++;
        if (cases[i].exp_msg)
            free(cases[i].exp_msg);
    }

    printf("\t\tTotal: %d, Passed: %d, Failed: %d\n", 
            ntotal, npass, nfail);
    *total += ntotal;
    *pass += npass;
    *fail += nfail;
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
                   && exp_msg && strcmp(buf, exp_msg) != 0) {
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
// endregion: set_opt_bit

// region: parse_opts
void test_parse_opts_short_single(int *total, int *pass, int *fail)
{
    int i, npass, nfail, exp_optind;
    unsigned exp_optstate;
    char opt[4], *exp_msg, *cmd_str, **argv;
    
    printf("\n\tsingle short opts\n"); 

    npass = nfail = 0;

    for (i = 0; i <= UCHAR_MAX; i++) {
        exp_optind = 2;
        exp_optstate = 0x1;
        exp_msg = NULL;

        sprintf(opt, "-%c", i);
        switch(i) {
            case 'S':
                exp_optstate <<= STRUCT;
                exp_msg = rus_doll_fmt(3, "freq_test: %s\n", OptReqsArg, opt);
                break;
            case 'l':
                exp_optstate <<= LONG;
                break;
            case 'f':
                exp_optstate <<= FLOAT;
                break;
            case 'd':
                exp_optstate <<= DOUBLE;
                break;
            case 'i':
                exp_optstate <<= INT;
                break;
            case 's':
                exp_optstate <<= SORT;
                break;
            case 'R':
                exp_optstate <<= RAW;
                break;
            case 'D':
                exp_optstate <<= DELIM;
                exp_msg = rus_doll_fmt(3, "freq_test: %s\n", OptReqsArg, opt);
                break;
            case 'a':
                exp_optstate <<= AGGR;
                break;
            case 'h':
                exp_msg = rus_doll_fmt(2, "freq_test: %s\n", UsageInfoStr);
                break;
            case '-':
                exp_optstate = 0;
                break;
            case '\0':
                exp_optind = 1;
                exp_optstate = 0;
                break;
            default:
                exp_msg = rus_doll_fmt(3, "freq_test: %s\n", InvOptStr, opt);
                exp_optstate = 0x0;
        }

        argv = create_argv(2, "./freq_test:", opt);
        cmd_str = concat_str_arr(argv, " ");

        printf("\t\tcmd: \"%s\", exp_optstate: %u: ", cmd_str, exp_optstate);
        fflush(stdout);
        
        if(test_parse_opts(2, argv, exp_optstate, 0, exp_optind, exp_msg))
            npass++;
        else
            nfail++;
        
        free(cmd_str);
        free(argv);
        if (exp_msg)
            free(exp_msg);
    }
    
    printf("\t\tTotal: %d, Passed: %d, Failed: %d\n", i, npass, nfail);
    *total += i;
    *pass += npass;
    *fail += nfail;
}

void test_parse_opts_short_combined_single(int *total, int *pass, int *fail)
{
    int i, npass, nfail;
    struct TestCase {
        int       argc;
        int       exp_size;
        int       exp_optind; // GNU getopt intializes optind to 1
        unsigned  exp_optstate;
        char      *exp_msg;
        char      **argv;
    };

    printf("\n\tshort opts combined separate\n"); 

    npass = nfail = 0;

    struct TestCase cases[] = {
    // Valid combinations
    {3, 0, 3, 0x28, NULL, 
      create_argv(3, "./freq_test", "-i", "-R")}, 
    
    {4, 0, 4, 0x0C, NULL, 
      create_argv(4, "./freq_test", "-D", "\"delim\"", "-R")},  
    
    {4, 0, 4, 0x112, NULL, 
      create_argv(4, "./freq_test", "-s", "-a", "-l")}, 
    
    {3, 0, 3, 0x88, NULL, 
      create_argv(3, "./freq_test", "-f", "-R")}, 
    
    {4, 10, 4, 0x208, NULL, 
      create_argv(4, "./freq_test", "-S", "10", "-R")}, 

    // Invalid combinations (mutually exclusive)
    {3, 0, 3, 0x00, rus_doll_fmt(3, "freq_test: %s\n", InvOptMutex, MutexOpts), 
      create_argv(3, "./freq_test", "-i", "-d")}, 
    
    {3, 0, 3, 0x00, rus_doll_fmt(3, "freq_test: %s\n", InvOptMutex, MutexOpts), 
      create_argv(3, "./freq_test", "-f", "-l")}, 
    
    {5, 0, 6, 0x00, rus_doll_fmt(3, "freq_test: %s\n", InvOptMutex, MutexOpts), 
      create_argv(5, "./freq_test", "-i", "-d", "-S", "5")}, 
    
    {4, 0, 4, 0x00, rus_doll_fmt(3, "freq_test: %s\n", InvOptMutex, MutexOpts), 
      create_argv(4, "./freq_test", "-S", "5", "-i")}, 
    
    {4, 0, 4, 0x00, rus_doll_fmt(3, "freq_test: %s\n", InvOptMutex, MutexOpts),
      create_argv(4, "./freq_test","-S", "5", "-d")},  

    // Missing required arguments
    {3, 0, 3, 0x00, rus_doll_fmt(3, "freq_test: %s\n", OptReqsQt, "-D"), 
      create_argv(3, "./freq_test", "-D", "-R")},  
    
    {3, 0, 3, 0x00, rus_doll_fmt(3, "freq_test: %s\n", OptReqsArg, "-S"), 
      create_argv(3, "./freq_test", "-S", "-R")},  

    // Option -S requires -R
    {4, 0, 4, 0x00, rus_doll_fmt(2, "freq_test: %s\n", OptSReqsArgR),
      create_argv(4, "./freq_test", "-S", "10", "-s")},  
    
    {4, 10, 4, 0x208, NULL, 
      create_argv(4, "./freq_test", "-S", "10", "-R")},  

    // Valid multiple short options
    {7, 0, 7, 0x3E, NULL, // 0000 0011 1110 
      create_argv(7, "./freq_test", "-i", "-R", "-s", "-a", "-D", "\",\"")},  
    {4, 0, 4, 0x98, NULL, 
      create_argv(5, "./freq_test", "-f", "-R", "-s")},

    {0, 0, 0, 0, NULL, NULL}  
    };

    for (i = 0; cases[i].argc != 0; i++) {
        printf("\t\tcmd: \"%s\" argc: %d, exp_optstate: %u, exp_optind: %d: ", 
                concat_str_arr(cases[i].argv, " "), cases[i].argc, 
                cases[i].exp_optstate, cases[i].exp_optind);
        fflush(stdout);
        if (test_parse_opts(cases[i].argc,cases[i].argv,cases[i].exp_optstate,
                     cases[i].exp_size, cases[i].exp_optind, cases[i].exp_msg))
            npass++;
        else
            nfail++;;
        if (cases[i].exp_msg)
            free(cases[i].exp_msg);
        free(cases[i].argv);
    }

    printf("\t\tTotal: %d, Passed: %d, Failed: %d\n", i, npass, nfail);

    *total += i;
    *pass += npass;
    *fail += nfail;    
}

void test_parse_opts_short_combined_concat(int *total, int *pass, int *fail)
{
    int i, npass, nfail;
    struct TestCase {
        int       argc;
        int       exp_size;
        int       exp_optind; // GNU getopt intializes optind to 1
        unsigned  exp_optstate;
        char      *exp_msg;
        char      **argv;
    };
    
    printf("\n\tshort opts combined together\n"); 

    npass = nfail = 0;

    struct TestCase cases[] = {
    // Valid combinations
    {2, 0, 2, 0x28, NULL, 
      create_argv(2, "./freq_test", "-iR")}, 
    
    {3, 0, 3, 0x0C, NULL, 
      create_argv(3, "./freq_test", "-RD", "\"delim\"")},  
    
    {2, 0, 2, 0x112, NULL, 
      create_argv(2, "./freq_test", "-sal")}, 
    
    {2, 0, 2, 0x88, NULL, 
      create_argv(2, "./freq_test", "-fR")}, 
    
    {3, 10, 3, 0x208, NULL, 
      create_argv(3, "./freq_test", "-RS", "10")}, 

    // Invalid combinations (mutually exclusive)
    {2, 0, 2, 0x00, rus_doll_fmt(3, "freq_test: %s\n", InvOptMutex, MutexOpts), 
      create_argv(2, "./freq_test", "-id")}, 
    
    {2, 0, 2, 0x00, rus_doll_fmt(3, "freq_test: %s\n", InvOptMutex, MutexOpts), 
      create_argv(2, "./freq_test", "-fl")}, 
    
    {3, 0, 3, 0x00, rus_doll_fmt(3, "freq_test: %s\n", InvOptMutex, MutexOpts), 
      create_argv(3, "./freq_test", "-idS", "5")}, 
   
    // Invalid opt ordering (req arg opt should come last) 
    {3, 0, 3, 0x00, rus_doll_fmt(3, "freq_test: %s\n", OptReqsArg, "-S"), 
      create_argv(3, "./freq_test", "-Si", "5")}, 
    
    {3, 0, 3, 0x00, rus_doll_fmt(3, "freq_test: %s\n", OptReqsArg, "-S"), 
      create_argv(3, "./freq_test","-Sd", "5")},  
    
    {3, 0, 3, 0x00, rus_doll_fmt(3, "freq_test: %s\n", OptReqsArg, "-S"), 
      create_argv(3, "./freq_test","-dfiSR", "5")},  
    
    // Missing required arguments
    {2, 0, 2, 0x00, rus_doll_fmt(3, "freq_test: %s\n", OptReqsQt, "-D"), 
      create_argv(2, "./freq_test", "-DR")},  
    
    {2, 0, 2, 0x00, rus_doll_fmt(3, "freq_test: %s\n", OptReqsArg, "-S"), 
      create_argv(2, "./freq_test", "-SR")},  

    // Option -S requires -R
    {3, 0, 3, 0x00, rus_doll_fmt(2, "freq_test: %s\n", OptSReqsArgR), 
      create_argv(3, "./freq_test", "-sS", "10")},  
    
    {3, 10, 3, 0x208, NULL, 
      create_argv(3, "./freq_test", "-RS", "10")},  

    // Valid multiple short options
    {3, 0, 3, 0x3E, NULL, // 0000 0011 1110 
      create_argv(3, "./freq_test", "-iRsaD", "\",\"")},  
    {2, 0, 2, 0x98, NULL, 
      create_argv(2, "./freq_test", "-fRs")},
    
    {0, 0, 0, 0, NULL, NULL} 
    };

    for (i = 0; cases[i].argc != 0; i++) {
         printf("\t\tcmd: \"%s\" argc: %d, exp_optstate: %u, exp_optind: %d: ", 
                concat_str_arr(cases[i].argv, " "), cases[i].argc, 
                cases[i].exp_optstate, cases[i].exp_optind);
        fflush(stdout);
        if (test_parse_opts(cases[i].argc,cases[i].argv,cases[i].exp_optstate,
                     cases[i].exp_size, cases[i].exp_optind, cases[i].exp_msg))
            npass++;
        else
            nfail++;

        if (cases[i].exp_msg)
            free(cases[i].exp_msg);        
        free(cases[i].argv);
    }

    printf("\t\tTotal: %d, Passed: %d, Failed: %d\n", i, npass, nfail);
    *total += i;
    *pass += npass;
    *fail += nfail;  
}

void test_parse_opts_long_single(int *total, int *pass, int *fail)
{
    int i, npass, nfail;
    struct TestCase {
        int       argc;
        int       exp_size;
        int       exp_optind; // GNU getopt intializes optind to 1
        unsigned  exp_optstate;
        char      *exp_msg;
        char      **argv;
    };
    
    printf("\n\tlong opts single\n"); 

    struct TestCase cases[] = {
    {2, 0, 0, 0x0, rus_doll_fmt(2, "freq_test: %s\n", UsageInfoStr), 
      create_argv(2, "./freq", "--help")},    

    {2, 0, 2, 0x2, NULL, 
      create_argv(2, "./freq", "--aggregate")},    

    {2, 0, 0, 0x0, rus_doll_fmt(3, "freq_test: %s\n", OptReqsArg, "--delim"), 
      create_argv(2, "./freq", "--delim")},    

    {2, 0, 2, 0x8, NULL, 
      create_argv(2, "./freq", "--raw")},    

    {2, 0, 2, 0x10, NULL, 
      create_argv(2, "./freq", "--sort")},    

    {2, 0, 2, 0x20, NULL, 
      create_argv(2, "./freq", "--int")},    

    {2, 0, 2, 0x40, NULL, 
      create_argv(2, "./freq", "--double")},    

    {2, 0, 2, 0x80, NULL, 
      create_argv(2, "./freq", "--float")},    

    {2, 0, 2, 0x100, NULL, 
      create_argv(2, "./freq", "--long")},    

    {2, 0, 0, 0x0, rus_doll_fmt(3, "freq_test: %s\n", OptReqsArg, "--struct"), 
      create_argv(2, "./freq", "--struct")},    

    {0, 0, 0, 0, NULL, NULL} 
    };

    npass = nfail = 0;
    for (i = 0; cases[i].argc != 0; i++) {
         printf("\t\tcmd: \"%s\" argc: %d, exp_optstate: %u, exp_optind: %d: ", 
                concat_str_arr(cases[i].argv, " "), cases[i].argc, 
                cases[i].exp_optstate, cases[i].exp_optind);
        fflush(stdout);
        if (test_parse_opts(cases[i].argc,cases[i].argv,cases[i].exp_optstate,
                     cases[i].exp_size, cases[i].exp_optind, cases[i].exp_msg))
            npass++;
        else
            nfail++;

        free(cases[i].argv);
        if (cases[i].exp_msg)
            free(cases[i].exp_msg);
    }

    printf("\t\tTotal: %d, Passed: %d, Failed: %d\n", i, npass, nfail);
    *total += i;
    *pass += npass;
    *fail += nfail; 
}
 
void test_parse_opts_long_combined_single(int *total, int *pass, int *fail)
{

}
 
void test_parse_opts_short_long_mixed(int *total, int *pass, int *fail)
{

}
 
int test_parse_opts(int argc, char **argv, unsigned exp_optstate, 
                    int exp_size, int exp_optind, char *exp_msg)
{
    pid_t pid;
    int status, pass, size, pipefd[2];
    unsigned opt_state;
    char *delim, *buf;

    delim = buf = NULL;
    size = 0;
    
    if (exp_msg == NULL)
        exp_msg = "";

    assert(pipe(pipefd) == 0);

    pid = fork();
    assert(pid >= 0);

    if (pid == 0) {
        create_pipe(pipefd);

        opt_state = parse_opts(argc, argv, &delim, &size);

        assert(opt_state == exp_optstate);
        assert(size == exp_size);
        assert(optind == exp_optind);

        if (delim)
            free(delim);

        exit(EXIT_SUCCESS);
    }

    close(pipefd[1]);
    wait(&status);
    read_pipe_to_buf(&buf, pipefd);

    if (WIFEXITED(status))
        // EXIT_FAILURE or EXIT_SUCCESS
        if (WEXITSTATUS(status) == 1 || WEXITSTATUS(status) == 0)
            pass = (strcmp(exp_msg, buf) == 0);
        else
            pass = 0;
    else if (WIFSIGNALED(status))
        pass = 0;
    else
        pass = 1;

    if (pass)
        printf("Pass\n");
    else if (exp_msg)
        printf("Failed: EXIT_CODE: %d, SIGNAL: %d, Expected: %s Actual: %s\n", 
                WEXITSTATUS(status), WTERMSIG(status), exp_msg, buf);
    else
        printf("Failed: EXIT_CODE: %d, SIGNAL: %d: %s\n", 
                WEXITSTATUS(status), WTERMSIG(status), buf);

    if (buf)
        free(buf);

    return pass > 0 ? 1 : 0;
}

// endregion: parse_opts

// region: helpers
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

// format strings and return one string, 
// assumes all str args have format info except the last one
// works backwards on the input args list 
// example usage: 
//    char *str = concat_with_fmt(3, "./freq: %s\n", "opt %s invalid", "-X");
//    printf(str);
//    $> ./freq: opt -X invalid
//    $> _
char *rus_doll_fmt(int n, ...) 
{
    int i, len;
    char *str, *nxt, **argv;
    va_list args;

    if (n <= 0)
        return NULL;

    va_start(args, n);
    argv = (char **) emalloc(n * sizeof(char *));
    for (len = i = 0; i < n; i++) {
        argv[i] = va_arg(args, char *);
        len += strlen(argv[i]);
    }
    va_end(args);


    str = (char *) emalloc((len * sizeof(char *)) + 1); // accounting for '\0'
    nxt = (char *) emalloc((len * sizeof(char *)) + 1); // accounting for '\0'
    str[0] = '\0';
    nxt[0] = '\0';

    for (i = n-1; i >= 0; i--) {
        if (!strlen(nxt)) {
            sprintf(nxt, "%s", argv[i]);
        } else {
            sprintf(str, argv[i], nxt);
            strcpy(nxt, str);
        }
    }
    free(argv);
    free(nxt);
    return str;
}
// endregion: helpers
