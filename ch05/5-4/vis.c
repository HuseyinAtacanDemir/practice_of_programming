#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "eprintf.h"

#define BUFSIZE 1024

enum { UNBUF, BUF };

void vis(FILE *fin, int buffered);

int main(int argc, char **argv)
{
    int i;
    FILE *fin;

    if (argc == 1)
        vis(stdin, UNBUF);
    else {
        for (i = 1; i < argc; i++) {
            if ((fin = fopen(argv[i], "rb")) == NULL)
                weprintf("can't open %s: ", argv[i]);
            else {
                vis(fin, BUF);
                fclose(fin);
            }
        }
    }
    return 0;
}

void vis(FILE *fin, int buffered) {
    int i, j, c;
    char buf[BUFSIZE];
    char non_print_char[5] = {'\\', 'X', ' ', ' ', '\0'};

    do {
        for (i = j = 0; (c = getc(fin)) != EOF && i < BUFSIZE - 4; ) {
            if (isprint(c) && c != '\\')
                buf[i++] = c;
            else {
                snprintf(non_print_char+2, 3, "%02X", c);
                memcpy(buf+i, non_print_char, 4);
                i += 4;
            }
            if (c == '\n' && !buffered) { // unbuf output mode, flush buf upon \n
                printf("%.*s\n\n", i-j, buf+j);
                j = i;
            }
        }
        if (buffered) // buf output mode, print buf when filled up or eof
            printf("%.*s", i, buf);
    } while (c != EOF);
}
