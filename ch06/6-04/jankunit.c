#include "jankunit.h"
#include "jankunit.internal.h"
#include <errno.h>
#include <signal.h>

#define COLOR_GREEN "\x1b[32m"
#define COLOR_RED "\x1b[31m"
#define COLOR_RESET "\x1b[0m"

#define BUFSIZE 1024

JankUnitContext *GLOBAL_CTX = NULL;

void init_ctx(void)
{
    GLOBAL_CTX = (JankUnitContext *) malloc(sizeof(JankUnitContext));

    for (int i = PROG; i <= TEST; i++)
        GLOBAL_CTX->cur_block[i] = NULL;

    //bufs uninitialized
    GLOBAL_CTX->bufs = NULL;
    
    // init pipes as closed
    for (int i = USR; i <= SYS; i++)
        for (int j = OUT; j <= ERR; j++)
            for (int k = READ_END; k <= WRITE_END; k++)    
                GLOBAL_CTX->pipes[i][j][k] = -1;
                
    // indent level init
    GLOBAL_CTX->indent = (int *) emalloc(sizeof(int));
    *(GLOBAL_CTX->indent) = 0;

    // other
    GLOBAL_CTX->flushed = UNFLUSHED;
    GLOBAL_CTX->is_forked = 0;
    GLOBAL_CTX->STATUS = 0;
    GLOBAL_CTX->EXIT_CODE = NOT_EXITED;
    GLOBAL_CTX->SIGNAL_CODE = NOT_SIGNALED;
}

void start_test_block(int block_type, const char *name_fmt, ...)
{
    va_list args;

    if(GLOBAL_CTX == NULL)
        eprintf("No test context was found! :");
    else if (GLOBAL_CTX->cur_block[block_type])
        eprintf("There is already a test in context! :");

    GLOBAL_CTX->cur_block[block_type] = (TestBlock *) emalloc(sizeof(TestBlock));
    
    GLOBAL_CTX->cur_block[block_type]->type = block_type;
    GLOBAL_CTX->cur_block[block_type]->total = 0;
    GLOBAL_CTX->cur_block[block_type]->passed = 0;
    GLOBAL_CTX->cur_block[block_type]->failed = 0;
    GLOBAL_CTX->cur_block[block_type]->failed_assert = 0;

    va_start(args, name_fmt);
    evasprintf(&(GLOBAL_CTX->cur_block[block_type]->name), name_fmt, args);
    va_end(args);

    print_with_indent("%s\n", GLOBAL_CTX->cur_block[block_type]->name);

    (*GLOBAL_CTX->indent)++;
}

void end_test_block(int block_type, int should_propagate)
{
    int is_test_failed;

    if(GLOBAL_CTX == NULL)
        eprintf("No test context was found! :");
    else if (GLOBAL_CTX->cur_block[block_type] == NULL)
        eprintf("No test in context was found! :");
    else if (GLOBAL_CTX->cur_block[block_type]->name == NULL)
        eprintf("No test name in context was found! :");
    
    is_test_failed = 0;
    
    (*GLOBAL_CTX->indent)--;
    if (GLOBAL_CTX->cur_block[block_type]->failed == 0) {
        print_result(COLOR_GREEN, "Total: %d, %d passed %d failed\n", 
                            GLOBAL_CTX->cur_block[block_type]->total, 
                            GLOBAL_CTX->cur_block[block_type]->passed, 
                            GLOBAL_CTX->cur_block[block_type]->failed);
    } else {
        is_test_failed = 1;
        print_result(COLOR_RED, "Total: %d, %d passed %d failed\n", 
                            GLOBAL_CTX->cur_block[block_type]->total, 
                            GLOBAL_CTX->cur_block[block_type]->passed, 
                            GLOBAL_CTX->cur_block[block_type]->failed);
    }     

    free(GLOBAL_CTX->cur_block[block_type]->name);
    GLOBAL_CTX->cur_block[block_type]->name = NULL;
    free(GLOBAL_CTX->cur_block[block_type]);
    GLOBAL_CTX->cur_block[block_type] = NULL;

    // propagate test success or failure to suite layer above
    if (should_propagate && is_test_failed)
        FAIL();
    else
        PASS();
}

// Helper function for printing with indentation
void print_with_indent(const char *fmt, ...) 
{
    va_list args;

    for (int i = 0; i < *GLOBAL_CTX->indent; i++)
        printf("    ");  // Four spaces per level

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

void vprint_with_indent(const char *fmt, va_list args)
{
    for (int i = 0; i < *GLOBAL_CTX->indent; i++)
        printf("    ");
    vprintf(fmt, args);
}

// Helper to print result with color
void print_result(const char *color, const char *fmt, ...) 
{
    va_list args;
    
    printf("%s", color);
    
    va_start(args, fmt);
    vprint_with_indent(fmt, args);
    va_end(args);

    printf("%s", COLOR_RESET);
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
    TestBlock   *sh_block;
    int         *sh_indent;

    sh_block = NULL;
    sh_indent = NULL;
    
    /*  
        The following values, if they exist, 
        should be made to use shared memory
        before a fork() call:
            GLOBAL_CTX->cur_block[PROG]
            GLOBAL_CTX->cur_block[SUITE]
            GLOBAL_CTX->cur_block[TEST]

    */
    for (int i = PROG; i <= TEST; i++) {
        if (GLOBAL_CTX->cur_block[i]) {
            sh_block = (TestBlock *) eshmalloc(sizeof(TestBlock));
            sh_block->type          = GLOBAL_CTX->cur_block[i]->type;
            sh_block->total         = GLOBAL_CTX->cur_block[i]->total;
            sh_block->passed        = GLOBAL_CTX->cur_block[i]->passed;
            sh_block->failed        = GLOBAL_CTX->cur_block[i]->failed;
            sh_block->failed_assert = GLOBAL_CTX->cur_block[i]->failed_assert;

            if (GLOBAL_CTX->cur_block[i]->name) {
                eshasprintf(&(sh_block->name), GLOBAL_CTX->cur_block[i]->name);
                free(GLOBAL_CTX->cur_block[i]->name);
                GLOBAL_CTX->cur_block[i]->name = NULL;
            } else {
                eprintf("Block with no name in context! :");   
            }
            free(GLOBAL_CTX->cur_block[i]);
            GLOBAL_CTX->cur_block[i] = NULL;

            GLOBAL_CTX->cur_block[i] = sh_block;
            sh_block = NULL;
        }
    }
    
    if (GLOBAL_CTX->indent) {
        sh_indent = (int *) eshmalloc(sizeof(int));
        *sh_indent = *GLOBAL_CTX->indent;
        
        free(GLOBAL_CTX->indent);
        GLOBAL_CTX->indent = NULL;

        GLOBAL_CTX->indent = sh_indent;
        sh_indent = NULL;
    } else {
        eprintf("No indent information in context! :");
    }
    /*
        BUFS will initialized in regular mem,
        forked process need not access BUFS
        of parent.
        Parent only ever needs initialized BUFS
        to copy pipes in from eventual children
    */
    if (GLOBAL_CTX->bufs) {
        for (int i = USR; i <= SYS; i++) {
            for (int j = OUT; j <= ERR; j++) {
                if (GLOBAL_CTX->bufs->bufs[i][j] != NULL) {
                    free(GLOBAL_CTX->bufs->bufs[i][j]);
                    GLOBAL_CTX->bufs->bufs[i][j] = NULL;
                }
                GLOBAL_CTX->bufs->sizes[i][j] = 0;
            }
        }
        free(GLOBAL_CTX->bufs);
        GLOBAL_CTX->bufs = NULL;
    }
    GLOBAL_CTX->bufs = (BUFS *) emalloc(sizeof(BUFS));
    for (int i = USR; i <= SYS; i++)
        for (int j = OUT; j <= ERR; j++) {
            GLOBAL_CTX->bufs->bufs[i][j] = NULL;
            GLOBAL_CTX->bufs->sizes[i][j] = 0;
        }

    // init pipes of the GLOBAL CTX
    for (int i = USR; i <= SYS; i++) {
        for (int j = OUT; j <= ERR; j++) {
            for (int k = READ_END; k <= WRITE_END; k++) {
                if (GLOBAL_CTX->pipes[i][j][k] != -1)
                    eprintf("Attempt to open already open pipe! :");
            }
            if (pipe(GLOBAL_CTX->pipes[i][j]))
                eprintf("Pipe failed! :");
        }
    }
                
    GLOBAL_CTX->flushed = UNFLUSHED;
    GLOBAL_CTX->is_forked = 1;
    GLOBAL_CTX->STATUS = 0;
    GLOBAL_CTX->EXIT_CODE = NOT_EXITED;
    GLOBAL_CTX->SIGNAL_CODE = NOT_SIGNALED;
}

// This will be called outside of the FORK block
void configure_ctx_post_fork()
{
    TestBlock   *block;
    int         *indent;

    block = NULL;
    indent = NULL;

    read_pipes_in_parent();
    wait(&(GLOBAL_CTX->STATUS));
    
    /*  
        The following values, if they exist, 
        should be made to use private memory
        before a fork() call, and their shared
        memory isntances should be shfreed:
            GLOBAL_CTX->cur_block[PROG]
            GLOBAL_CTX->cur_block[SUITE]
            GLOBAL_CTX->cur_block[TEST]

    */
    for (int i = PROG; i <= TEST; i++) {
        if (GLOBAL_CTX->cur_block[i]) {
            block = (TestBlock *) emalloc(sizeof(TestBlock));
            block->type          = GLOBAL_CTX->cur_block[i]->type;
            block->total         = GLOBAL_CTX->cur_block[i]->total;
            block->passed        = GLOBAL_CTX->cur_block[i]->passed;
            block->failed        = GLOBAL_CTX->cur_block[i]->failed;
            block->failed_assert = GLOBAL_CTX->cur_block[i]->failed_assert;

            if (GLOBAL_CTX->cur_block[i]->name) {
                easprintf(&(block->name), GLOBAL_CTX->cur_block[i]->name);
                eshfree(GLOBAL_CTX->cur_block[i]->name);
                GLOBAL_CTX->cur_block[i]->name = NULL;

                GLOBAL_CTX->cur_block[i]->name = block->name;
            } else {
                eprintf("Block with no name in context! :");
            }
            eshfree(GLOBAL_CTX->cur_block[i]);
            GLOBAL_CTX->cur_block[i] = NULL;

            GLOBAL_CTX->cur_block[i] = block;
            block = NULL;
        }
    }
    
    
    if (GLOBAL_CTX->indent) {
        indent = (int *) emalloc(sizeof(int));
        *indent = *GLOBAL_CTX->indent;

        eshfree(GLOBAL_CTX->indent);
        GLOBAL_CTX->indent = NULL;

        GLOBAL_CTX->indent = indent;
        indent = NULL;
    } else {
        eprintf("No indent information in context! :");
    }
    /*
        During cleanup sys out buffers, which hold
        printed output of the checks implemented
        within the forked block, will be printed
        to stdout of parent.
        
        If sys errors exist, sys errs will be printed 
        and the program will exit.

        After sys out and err printing,
        sys bufs will be freed.
        
        USR out and ERR will stay put for use in
        assertion and expectation macros
    */

    // TODO bufs could be refactored to be test_block specific
    if (GLOBAL_CTX->bufs && GLOBAL_CTX->bufs->bufs[SYS][OUT]) {
        if (strlen(GLOBAL_CTX->bufs->bufs[SYS][OUT]) > 0) {
            printf("%s", GLOBAL_CTX->bufs->bufs[SYS][OUT]);
            fflush(stdout);
        }
        free(GLOBAL_CTX->bufs->bufs[SYS][OUT]);
        GLOBAL_CTX->bufs->bufs[SYS][OUT] = NULL;
    }

    if (GLOBAL_CTX->bufs && GLOBAL_CTX->bufs->bufs[SYS][ERR] 
      && strlen(GLOBAL_CTX->bufs->bufs[SYS][ERR]) > 0) {
        fprintf(stderr, "%s", GLOBAL_CTX->bufs->bufs[SYS][ERR]);
        exit(EXIT_FAILURE);
    }

    // init pipes of the GLOBAL CTX
    GLOBAL_CTX->flushed = UNFLUSHED;
    GLOBAL_CTX->is_forked = 0;

    GLOBAL_CTX->EXIT_CODE = (WIFEXITED(GLOBAL_CTX->STATUS) ? 
                              WEXITSTATUS(GLOBAL_CTX->STATUS) : NOT_EXITED);
    GLOBAL_CTX->SIGNAL_CODE = (WIFSIGNALED(GLOBAL_CTX->STATUS) ? 
                                WTERMSIG(GLOBAL_CTX->STATUS) : NOT_SIGNALED);
}

void dup2_usr_pipes()
{
    for (int i = USR; i <= SYS; i++)
        for (int j = OUT; j <= ERR; j++) {
            if (GLOBAL_CTX->pipes[i][j][READ_END] != -1) {
                close(GLOBAL_CTX->pipes[i][j][READ_END]);
                GLOBAL_CTX->pipes[i][j][READ_END] = -1;
            }
        }

    dup2(GLOBAL_CTX->pipes[USR][OUT][WRITE_END], STDOUT_FILENO);
    dup2(GLOBAL_CTX->pipes[USR][ERR][WRITE_END], STDERR_FILENO);
}

void flush_usr_pipes_and_dup2_sys_pipes()
{
    fflush(stdout);
    
    if (GLOBAL_CTX->pipes[USR][OUT][WRITE_END] != -1) {
        close(GLOBAL_CTX->pipes[USR][OUT][WRITE_END]);
        GLOBAL_CTX->pipes[USR][OUT][WRITE_END] = -1;
    }
    if (GLOBAL_CTX->pipes[USR][ERR][WRITE_END] != -1) {
        close(GLOBAL_CTX->pipes[USR][ERR][WRITE_END]);
        GLOBAL_CTX->pipes[USR][ERR][WRITE_END] = -1;
    }

    dup2(GLOBAL_CTX->pipes[SYS][OUT][WRITE_END], STDOUT_FILENO);
    dup2(GLOBAL_CTX->pipes[SYS][ERR][WRITE_END], STDERR_FILENO);
}

//TODO: make sure this is registered as at exit
void flush_and_close_all_pipes()
{
    fflush(stdout);
    close_all_pipes();
}

void close_all_pipes() 
{
    for (int i = USR; i <= SYS; i++)
        for (int j = OUT; j <= ERR; j++)
            for (int k = READ_END; k <= WRITE_END; k++)
                if (GLOBAL_CTX && GLOBAL_CTX->pipes[i][j][k] != -1) {
                    close(GLOBAL_CTX->pipes[i][j][k]);
                    GLOBAL_CTX->pipes[i][j][k] = -1;
                }
}

void read_pipes_in_parent()
{
    // close write ends of all pipes in parent
    for (int i = USR; i <= SYS; i++)
        for (int j = OUT; j <= ERR; j++)
            if (GLOBAL_CTX->pipes[i][j][WRITE_END] != -1) {
                close(GLOBAL_CTX->pipes[i][j][WRITE_END]);
                GLOBAL_CTX->pipes[i][j][WRITE_END] = -1;
            }

    read_pipes_to_bufs();

    // after reading, close read ends of all pipes in parent
    for (int i = USR; i <= SYS; i++)
        for (int j = OUT; j <= ERR; j++)
            if (GLOBAL_CTX->pipes[i][j][READ_END] != -1) {
                close(GLOBAL_CTX->pipes[i][j][READ_END]);
                GLOBAL_CTX->pipes[i][j][READ_END] = -1;
            }
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
                if (nread[i][j] > 0) {
                    char **cur_buf = &(GLOBAL_CTX->bufs->bufs[i][j]);
                    int *cur_size = &(GLOBAL_CTX->bufs->sizes[i][j]);
            
                    if (*cur_buf == NULL) {
                        *cur_buf = (char *) emalloc((sizeof(char) * BUFSIZE));
                        *cur_size = BUFSIZE;
                    } else if ((nread[i][j] + ntot[i][j]) > *cur_size) {
                        *cur_buf = (char *) erealloc(*cur_buf, (*cur_size * 2));    
                        *cur_size *= 2;
                    }
                    memcpy(((*cur_buf) + ntot[i][j]), temp_bufs[i][j], nread[i][j]);
                    ntot[i][j] += nread[i][j];
                }
            }    
    }

    // NULL terminate bufs that exist
    for (int i = USR; i <= SYS; i++)
        for (int j = OUT; j <= ERR; j++)
            if (GLOBAL_CTX->bufs && GLOBAL_CTX->bufs->bufs[i][j] )
                GLOBAL_CTX->bufs->bufs[i][j][ntot[i][j]] = '\0';

}

void handle_signal(int sig)
{
    char *msg;
    int len = 0;
    int err_no = errno;

    if (sig == SIGSEGV)
        msg = "\nSIGSEGV in child\n";
    else if (sig == SIGBUS)
        msg = "\nSIGBUS in child\n";
    else
        msg = "\nSome signale in child\n";

    for (char *p = msg, len = 0; *p != '\0'; p++, len++)
      ;

    //  write a message detailing the signal 
    //  and close all open pipes
    write(STDOUT_FILENO, msg, len+1);
    close_all_pipes();

    //after closing pipes, restore default sig handler
    struct sigaction sa_dfl;
    sa_dfl.sa_handler = SIG_DFL;
    sigemptyset(&sa_dfl.sa_mask);
    sa_dfl.sa_flags = 0;

    // reset the errno and raise the signal again 
    // for the default handler to handle
    errno = err_no;
    raise(sig);
}

void handle_all_catchable_signals()
{
    sigset_t set;

    struct sigaction sa_custom;
    sa_custom.sa_handler = handle_signal;
    sigemptyset(&sa_custom.sa_mask);
    sa_custom.sa_flags = SA_RESTART;

    //256 because, well, are there more signals??
    for (int i = 0; i < 256; i++) {
        sigfillset(&set);
        if (sigismember(&set, i) && i != SIGKILL && i != SIGSTOP && i != SIGTRAP) {
            sigaction(i, &sa_custom, NULL);
        }
    }
}
