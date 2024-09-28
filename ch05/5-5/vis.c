#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "eprintf.h"

#define BUFSIZE 1024
#define NOFOLD 0

enum { NOBUF, BUFFED };

void  vis           (FILE *fin, int isbuffed, int maxcol, char *options);
int   visprint      (char c, char *options);
int   iswholenumber (char *);
int   printf_fold   (char *str, int size, int maxcol, int curcol);

int main(int argc, char **argv)
{
    int i, j, n_options, maxcol;
    FILE *fin;
    char *options = NULL;

    setprogname("vis");

    maxcol = NOFOLD;
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
            case 'f':
            case 'F':
                if (j++ == 1 && ++i < argc && iswholenumber(argv[i])) {
                    maxcol = (int) atoi(argv[i]);
                    j = strlen(argv[i]);
                    if (maxcol < 5)
                        eprintf("can't fold lines to less than 5 cols");
                    break;
                } // otherwise fall through to usage
            default:
                eprintf("usage: ./vis -[tn] -[F foldwidth] file1 ...");
            }
        }
        if (j == 1)
            eprintf("usage: ./vis -[tn] -[F foldwidth] file1 ...");
    }
    
    if (i == argc) {
        vis(stdin, NOBUF, maxcol, options);
        return 0;
    }   

    for ( ; i < argc; i++) {
        if ((fin = fopen(argv[i], "rb")) == NULL)
            weprintf("can't open %s: ", argv[i]);
        else {
            vis(fin, BUFFED, maxcol, options);
            fclose(fin);
        }
    }
    return 0;
}

void vis(FILE *fin, int isbuffed, int maxcol, char *options) {
    int i, j, c, curcol;
    char hex_str[5]   = {'\\', 'x', ' ', ' ', '\0'};
    char tab_space[5] = {' ', ' ', ' ', ' ', '\0'};
    char buf[BUFSIZE];
    
    curcol = 0;
    do {
        for (i = j = 0; (c = getc(fin)) != EOF && i < BUFSIZE - 4; ) {
            if (c != '\t' && visprint(c, options)) {
                buf[i++] = c;
            } else if (c == '\t' && visprint(c, options)){
                memcpy(buf+i, tab_space, 4);
                i += 4;
            } else { // convert c to 2 digit hex, put into hex_str
                snprintf(hex_str+2, 3, "%02X", c);
                memcpy(buf+i, hex_str, 4);
                i += 4;
            } 

            // NOBUF mode, flush buf upon \n
            if (c == '\n' && !isbuffed) { 
                curcol = printf_fold(buf+j, i-j, maxcol, curcol);
                break;
            }
        }
        // BUFFED mode, print buf when filled or EOF
        if (isbuffed)
            curcol = printf_fold(buf, i, maxcol, curcol);
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

int iswholenumber(char *s)
{
    for ( ; *s >= '0' && *s <= '9'; s++)
        ;
    return (*s == '\0' ? 1 : 0);
}

int printf_fold(char *str, int size, int maxcol, int curcol)
{
    int i, j, nsym;
    char *sym = "\\$";

    if (maxcol == NOFOLD) {
        printf("%.*s", str, size);
        return 0;
    }

    nsym = strlen(sym);    
    for (i = j = 0; i < size; i++)
        if (str[i] == '\n') {
            printf("%.*s\n", i - j, str+j);
            j = i + 1;
            curcol = 0;
        } else if (curcol + (i-j+1) + nsym >= maxcol) {
            printf("%.*s%s\n", maxcol-curcol-nsym, str+j, sym);
            j = i + 1;
            curcol = 0;
        }
    
    if (j >= size)
        return ((maxcol != NOFOLD) ? curcol : 0);
        
    printf("%.*s", size-j, str+j);
    curcol += size - j;
    
    return ((maxcol != NOFOLD) ? curcol : 0);
}
