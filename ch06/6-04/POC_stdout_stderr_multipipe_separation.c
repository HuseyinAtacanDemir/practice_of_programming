#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define BUFSIZE 1024

enum { READ_END, WRITE_END };
enum { OUT, ERR };
enum { USER, SYS };

typedef struct BUFS BUFS;
struct BUFS 
{
    int   pipes[2][2][2];
    char  *bufs[4];
    int   sizes[4];
};

BUFS *init_bufs(void);
void destroy_bufs(BUFS *b);
void read_pipes_into_bufs(BUFS *b);

void init_pipes(BUFS *b)
{
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            if(pipe(b->pipes[i][j])) {
                fprintf(stderr, "failed pipe\n");
                exit(1);
            }
}

void close_all_read_ends(BUFS *b) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            close(b->pipes[i][j][READ_END]);
        }
    }
}

void redirect_err_out_of_fn(BUFS *b)
{
    dup2(b->pipes[USER][OUT][WRITE_END], STDOUT_FILENO);
    dup2(b->pipes[USER][ERR][WRITE_END], STDERR_FILENO);
}

void flush_fn_out_and_redirect_sys_pipes(BUFS *b)
{

        fflush(stdout);
        close(b->pipes[USER][OUT][WRITE_END]);
        close(b->pipes[USER][ERR][WRITE_END]);
        
        dup2(b->pipes[SYS][OUT][WRITE_END], STDOUT_FILENO);
        dup2(b->pipes[SYS][ERR][WRITE_END], STDERR_FILENO);
}

void flush_sys_out_and_close_sys_pipes(BUFS *b)
{
    fflush(stdout);
    close(b->pipes[SYS][OUT][WRITE_END]);
    close(b->pipes[SYS][ERR][WRITE_END]);
}

void read_pipes_in_parent_and_wait(BUFS *b, int *status)
{
    close(b->pipes[USER][OUT][WRITE_END]);
    close(b->pipes[USER][ERR][WRITE_END]);
    close(b->pipes[SYS][OUT][WRITE_END]);
    close(b->pipes[SYS][ERR][WRITE_END]);

    read_pipes_into_bufs(b);  

    close(b->pipes[USER][OUT][READ_END]);
    close(b->pipes[USER][ERR][READ_END]);
    close(b->pipes[SYS][OUT][READ_END]);
    close(b->pipes[SYS][ERR][READ_END]);

    wait(status);
}

int main(void)
{
    pid_t pid;
    int status;

    BUFS *b = init_bufs();
    init_pipes(b);
    
    pid = fork();

    if (pid == 0) {
        int i;
        close_all_read_ends(b);
        redirect_err_out_of_fn(b);
        for (i = 0; i < 1000; i++) {
            printf("%d ", i);
            fprintf(stderr, "%d ", i);
        }
        flush_fn_out_and_redirect_sys_pipes(b);
        for ( ; i < 2000; i++) {
            printf("%d ", i);
            fprintf(stderr, "%d ", i);
        }
        flush_sys_out_and_close_sys_pipes(b);
        exit(0);
    }

    read_pipes_in_parent_and_wait(b, &status);

    printf("USER OUT:\n%s\n", b->bufs[0]);
    printf("USER ERR:\n%s\n", b->bufs[1]);
    printf("SYS OUT:\n%s\n", b->bufs[2]);
    printf("SYS ERR:\n%s\n", b->bufs[3]);

    destroy_bufs(b);
    return 0;
}

void read_pipes_into_bufs(BUFS *b)
{
    char temp_bufs[4][BUFSIZE];
    int nread_tot[4] = { 0, 0, 0, 0 };
    int nread[4] = { 0, 0, 0, 0 };
    while (
        (nread[0] = read(b->pipes[USER][OUT][READ_END], temp_bufs[0], BUFSIZE)) > 0 ||
        (nread[1] = read(b->pipes[USER][ERR][READ_END], temp_bufs[1], BUFSIZE)) > 0 ||
        (nread[2] = read(b->pipes[SYS][OUT][READ_END], temp_bufs[2], BUFSIZE)) > 0 ||
        (nread[3] = read(b->pipes[SYS][ERR][READ_END], temp_bufs[3], BUFSIZE)) > 0 
    ) {
        for (int i = 0; i < 4; i++) {
            if (b->bufs[i] == NULL) {
                b->bufs[i] = (char *) malloc(sizeof(char) * BUFSIZE);
                b->sizes[i] = BUFSIZE;
            } else if ((nread[i] + nread_tot[i]) > b->sizes[i]) {
                b->sizes[i] *= 2;
                b->bufs[i] = realloc(b->bufs[i], b->sizes[i]);
            }
            memcpy( (b->bufs[i] + nread_tot[i]), temp_bufs[i], nread[i]);
            nread_tot[i] += nread[i];
        }
    }
    for (int i = 0; i < 4; i++)
        if (b->bufs[i] != NULL)
            b->bufs[i][nread_tot[i]] = '\0';
}

BUFS *init_bufs()
{
    BUFS *b = (BUFS *) malloc(sizeof(BUFS));
    for (int i = 0; i < 4; i++) {
        b->bufs[i] = NULL;
        b->sizes[i] = 0;
    } 
    return b;
}

void destroy_bufs(BUFS *b)
{
    for (int i = 0; i < 4; i++) {
        if (b->bufs[i] != NULL)
            free(b->bufs[i]);
    }
    free(b);
}
