#include "csv.h"
#include <string.h>

char buf[200];        // input line buffer
char *field[20];       // fields

// unquote: remove leading and trailing quote
char *unquote(char *p)
{
    if (p[0] == '"') {
        if (p[strlen(p) - 1] == '"')
            p[strlen(p)-1] = '\0';
        p++;
    }
    return p;
}

// csvgetline: read and parse line, return field count
// sample input: "LU",82.25,"11/4/1998","2:19PM",+4.0625
int csvgetline(FILE *fin)
{
    int nfield;
    char *p, *q;

    if (fgets(buf, sizeof(buf), fin) == NULL)
        return -1;
    nfield = 0;
    for (q = buf; (p=strtok(q, ",\n\r")) != NULL; q = NULL)
        field[nfield++] = unquote(p);
    return nfield;
}

