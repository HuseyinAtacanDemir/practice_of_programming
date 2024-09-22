#include "csv_lib.h"

enum { NOMEM = -2 };         

static char   *line   = NULL;  
static char   *sline  = NULL;  
static char   **field = NULL;  

static char fieldsep[] = ",";

static int    maxline  = 0;     
static int    maxfield = 0;     
static int    nfield   = 0;

// csvgetline: get one line, grow as needed
// sample input: "LU",86.25,"11/4/1998","2:19PM",+4.0625
char *csvgetline(FILE *fin)
{
    int i, c;
    char *newl, *news;

    if (line == NULL) {           // allocate on first call
        maxline = maxfield = 1;
        line = (char *) malloc(maxline);
        sline = (char *) malloc(maxline);
        field = (char **) malloc(maxfield*sizeof(field[0]));
        if (line == NULL || sline == NULL || field == NULL) {
            reset();
            return NULL;
        }
    }
    for (i=0; (c=getc(fin))!=EOF && !endofline(fin,c); i++) {
        if (i >= maxline-1) {     // grow line
            maxline *= 2;         // double current size
            newl = (char *) realloc(line, maxline);
            news = (char *) realloc(sline, maxline);
            if (newl == NULL || news == NULL) {
                reset();
                return NULL;
            }
            line = newl;
            sline = news;
        }
        line[i] = c;
    }
    line[i] = '\0';
    if (split() == NOMEM) {
        reset();
        return NULL;
    }
    return (c == EOF && i == 0) ? NULL : line;
}

// csvfield: return ptr to the n-th field
char *csvfield(int n)
{
    if (n < 0 || n >= nfield)
        return NULL;
    return field[n];
}

// csvnfield: return number of fields
int csvnfield(void)
{
    return nfield;
}

// reset: set variables back to starting values
static void reset(void)
{
    free(line);
    free(sline);
    free(field);
    line = NULL;
    sline = NULL;
    field = NULL;
    maxline = maxfield = nfield = 0;
}


// endofline: check for and consume \r, \n, \r\n, or EOF
static int endofline(FILE *fin, int c)
{
    int eol;

    eol = (c=='\r' || c=='\n');
    if (c == '\r') {
        c = getc(fin);
        if (c != '\n' && c != EOF)
            ungetc(c, fin); // read too far, put c back
    }
    return eol;
}



// split: split line into fields
static int split(void)
{
    char *p, **newf;
    char **sepp;      // ptr to temp separator char
    int sepc;         // temp spearator char

    nfield = 0;
    if (line[0] == '\0')
        return 0;
    strcpy(sline, line);
    p = sline;

    do {
        if (nfield >= maxfield) {
            maxfield *= 2;          // double current size
            newf = (char **) realloc(field,
                        maxfield * sizeof(field[0]));
            if (newf == NULL)
                return NOMEM;
            field = newf;
        }
        if (*p == '"')
            sepp = advquoted(++p);    // skip initial quote
        else
            sepp = p + strcspn(p, fieldsep);
        sepc = sepp[0];
        sepp[0] = '\0';               // terminate field
        field[nfield++] = p;
        p = sepp + 1;
    } while (sepc == ',');
    
    return nfield;
}


// advquoted: quoted field; return ptr to next separator
static char *advquoted(char *p)
{
    int i, j;

    for (i = j = 0; p[j] != '\0'; i++, j++) {
        if (p[j] == '"' && p[++j] != '"') {
            // copy up to next separator or \0
            int k = strcspn(p+j, fieldsep);
            memmove(p+i, p+j, k);
            i += k;
            j += k;
            break; 
        }
        p[i] = p[j];
    }
    p[i] = '\0';
    return p + j;
}


