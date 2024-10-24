#include "eread.h"

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "eprintf.h"

int e_getline(char *buf, int bufsize, char **ln, int bufseek)
{
    int len;
    char *c;

    if (bufseek >= bufsize)
        return -1;

    *ln = (char *)(buf+bufseek);
    c = *ln;
    for (len = 0; (bufseek+len) < bufsize && *c != '\n'; c++, len++)
        ;
    
    return len + 1;
}

// e_readline: reads a file descriptor using a char **buffer line by line
//              sets the cahr **ln pointer to the start of a line, where line
//              defined as a string of bytes (any bytes) upto and including \n
//
//              returns line length, or -1 if end of buffer.
//              exits with error message if failure.
int ea_readline(int fd, char **buf, int *nbuf_allocd, char **ln, int bufseek)
{
    if (*buf == NULL)
        *nbuf_allocd = ea_read_buf(fd, buf);
    
    return e_getline(*buf, *nbuf_allocd, ln, bufseek);
}

// ea_read_buf: reads a file descriptor into a char **buffer.
//              Allocates memory for the buffer and updates the buffer
//              so that it only has only ntotal_read bytes of memory allocated.
//              Does NOT terminate buffer with a NULL terminator
//              returns total number of allocated bytes. 
//              exits with error message if error
//    What to do if the file is, say, 100 GB?
int ea_read_buf(int fd, char **buf)
{
    int nbuf_allocd, nbuf_unused, ntotal_read, ncur_read;

    nbuf_allocd = nbuf_unused = E_READ_BUF_CHUNK_SIZE;
    ntotal_read = 0;

    *buf = (char *) emalloc(nbuf_allocd);
    
    // while read returns a value bigger than 0:
    //    we check if we have filled our buf, if so we resize buf
    while ((ncur_read = read(fd, (*buf + ntotal_read), nbuf_unused)) > 0) {
        // resize, buffer was not big enough
        if (ncur_read == nbuf_unused) {
            nbuf_allocd *= 2;
            void *new_buf = erealloc(*buf, nbuf_allocd);
            *buf = new_buf;
        }
        ntotal_read += ncur_read;
        nbuf_unused = (nbuf_allocd-ntotal_read);
    }
    if (ncur_read == -1)
        eprintf("error while reading file:");

    if (nbuf_unused) {
        nbuf_allocd = ntotal_read;
        void *new_buf = (char *) erealloc(*buf, nbuf_allocd);
        *buf = new_buf;
    }
    
    return nbuf_allocd;
}
