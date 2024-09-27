#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "eprintf.h"

#define BUFSIZE 1024
#define MINLEN  5

void strings(char *name, int minlen, FILE *fin);

int main(int argc, char **argv)
{
    int i, option, minlen;
    FILE *fin;

    setprogname("strings");
    if (argc == 1 || ((option=!strcmp(argv[1],"-l")) && argc <= 3) )
        eprintf("usage: strings [-l minlen] file1 ...");
    else {
        minlen = MINLEN;
        if (option)
            minlen = atoi(argv[2]);
        i = option ? 3 : 1;
        for (; i < argc; i++) {
            if ((fin = fopen(argv[i], "rb")) == NULL)
                weprintf("can't open %s:", argv[i]);
            else {
                strings(argv[i], minlen, fin);
                fclose(fin);
            }
        }
    }
    return 0;
}

void strings(char *name, int minlen, FILE *fin)
{
    int c, i;
    char buf[BUFSIZE];

    do {
        for (i = 0; (c = getc(fin)) != EOF; ) {
            if (!isprint(c))
                break;
            buf[i++] = c;
            if (i >= BUFSIZE)
                break;
        }
        if (i >= minlen)
            printf("%s: %.*s\n", name, i, buf);
    } while (c != EOF);
}
