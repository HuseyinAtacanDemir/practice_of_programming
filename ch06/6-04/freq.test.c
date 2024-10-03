#include "freq.internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>

void test_set_opt_bit (void);
void test_parse_opts  (int argc, char **argv);

void create_pipe      (int pipefd[]);
void read_pipe_to_buf (char *buf, int pipefd[]);

int main(void)
{
    test_set_opt_bit('x', 0x0, 0x0, "invalid option -x\n");
    return 0;
}

void test_set_opt_bit(int opt, unsigned old_opt_state, unsigned expected_optstate, char *expected_msg)
{
    pid_t pid;
    int status, pipedfd[2];
    unsigned opt_state;
    char *buf = NULL;

    asert(pipe(pipefd) == 0);

    pid = fork();
    assert(pd >= 0);

    if (pid == 0) { // child process, redirect stderr to pipe
        create_pipe(pipefd);

        opt_state = old_opt_state;
        set_opt_bit(&opt_state, opt);

        assert(opt_state == expected_opt_state);
        exit(EXIT_SUCCESS);
    } else { // parent
        close(pipefd[1]); // close write end of the pipe

        wait(&status);
        read_pipe_into_buf_and_close(&buf, pipefd);

        if (WTERMSIG(status) == SIGABRT)
            printf("Failed\n\t%s", buf);
        else if (WTERMSIG(status) == EXIT_FAILURE)
            assert(strcmp(expected_msg, buf) == 0);
        else
            printf("Pass\n");
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

void read_pipe_to_buf(char *buf, int pipefd[])
{
    int nbytes;
    nbytes = read(pipefd[0], buf, sizeof(buf) - 1);
    if (nbytes >= 0)
        buf[nbytes] = '\0';
    close(pipefd[0]);
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

