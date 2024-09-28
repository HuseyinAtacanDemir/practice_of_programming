#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "eprintf.h"

#define BUFSIZE 1024

enum { UNBUF, BUF };

void  vis(FILE *fin, int buffered, char *options);
int   visprint(char c, char *options);

int main(int argc, char **argv)
{
    int i, j, n_options;
    FILE *fin;
    char *options = NULL;

    n_options = 0;
    options = (char *) calloc(3, sizeof(char));

    for (i = 1; i < argc && argv[i][0] == '-'; i++) {
        for (j = 1; j < strlen(argv[i]); j++) {
            switch(argv[i][j]) {
                case 'n':
                    options[n_options++] = '\n';
                    break;
                case 't':
                    options[n_options++] = '\t';
                    break;
                default:
                    eprintf("usage: ./vis [-f fold_column_number] -[t][n] file1 ...");
            }
        }
        if (j == 1)
            eprintf("usage: ./vis [-f fold_column_number] -[t][n] file1 ...");
    }

    
    //if (argv[i][0] == '-')
        //eprintf("usage: ./vis [-f fold_column_number] -[t][n] file1 ...");
    
    if (i == argc) {
        vis(stdin, UNBUF, options);
        return 0;
    }   

    for ( ; i < argc; i++) {
        if ((fin = fopen(argv[i], "rb")) == NULL)
            weprintf("can't open %s: ", argv[i]);
        else {
            vis(fin, BUF, options);
            fclose(fin);
        }
    }
    return 0;
}

void vis(FILE *fin, int buffered, char *options) {
    int i, j, c;
    char buf[BUFSIZE];
    char non_print_char[5] = {'\\', 'x', ' ', ' ', '\0'};

    do {
        for (i = j = 0; (c = getc(fin)) != EOF && i < BUFSIZE - 4; ) {
            if (visprint(c, options))
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

int visprint(char c, char *options)
{
    if (c == '\\')
        return 0;
    if (options == NULL)
        return isprint(c);
    if (c == '\t' && strchr(options, '\t'))
        return 1;
    if (c == '\n' && strchr(options, '\n'))
        return 1;
    
    return isprint(c);
}

