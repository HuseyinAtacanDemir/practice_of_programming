#ifndef EREAD_H
#define EREAD_H

#define E_READ_BUF_CHUNK_SIZE  (256*1024) // 256 kb

extern int e_getline    (char *buf, int bufsize, char **ln, int bufseek);
extern int ea_readline  (int fd, char **buf, int *nbuf_allocd, char **ln, 
                          int bufseek);
extern int ea_read_buf  (int fd, char **buf);

#endif
