#ifndef JANKUNIT_INTERNAL_H
#define JANKUNIT_INTERNAL_H

#include <stdio.h>
#include <string.h>

enum { USR, SYS };
enum { OUT, ERR };
enum { READ_END, WRITE_END };
enum { UNINITIALIZED, FLUSHED, UNFLUSHED };
enum { PROG, SUITE, TEST };

typedef struct TestBlock TestBlock;
struct TestBlock {
    char *name;
    int type;
    int total;
    int passed;
    int failed;
    int failed_assert;
};

typedef struct BUFS BUFS;
struct BUFS {
    int sizes[2][2];
    char *bufs[2][2];
};

typedef struct JankUnitContext JankUnitContext;
struct JankUnitContext {
    TestBlock   *cur_block[3];
    BUFS        *bufs;
    
    int         pipes[2][2][2];

    int         *indent;
    
    int         flushed;
    int         in_fork;
    int         STATUS;
    int         EXIT_CODE;
    int         SIGNAL_CODE;    
};

void print_with_indent  (const char *fmt, ...);
void vprint_with_indent (const char *fmt, va_list args);
void print_result       (const char *color, const char *fmt, ...);
void vprint_result      (const char *color, const char *fmt, va_list args);

void configure_ctx_pre_fork             (void);
void configure_ctx_post_fork            (void);

void dup2_usr_pipes                     (void);
void flush_usr_pipes_and_dup2_sys_pipes (void);
void flush_and_close_all_pipes          (void);
void close_all_pipes                    (void);

void read_pipes_in_parent               (void);
void read_pipes_to_bufs                 (void);

void handle_all_catchable_signals       (void);
void handle_signal                      (int);

#endif
