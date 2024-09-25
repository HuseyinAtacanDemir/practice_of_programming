#include "csv.h"
#include <string.h>

#if defined(PROTO)
char buf[400];        // input line buffer
char *field[20];       // fields

// unquote: remove leading and trailing quote
char *unquote(char *p)
{
    char *q;
    if (p[0] == '"') {
        if (p[strlen(p) - 1] == '"')
            p[strlen(p)-1] = '\0';
        p++;
    }
    while (p[0] == ' ')
      p++;
    for (q = p + strlen(p)-1; *q == ' ' && q >= p; q--) {
        *q = '\0';
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

#elif defined(LIB)

#include <stdlib.h>

enum { NOMEM = -2 };         

static char *line       = NULL;  
static char *sline      = NULL;  
static char **field     = NULL;  
static char fieldsep[]  = ",";

static int  maxline  = 0;     
static int  maxfield = 0;     
static int  nfield   = 0;


static int    endofline   (FILE *fin, int c);
static int    split       (void);
static char   *advquoted  (char *p);
static void   reset       (void);


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
    char  *p, **newf;
    char  *sepp;      // ptr to separator char
    int   sepc;       // temp separator char
    int   pre_ws;     // trailing white space count in prefix
    int   post_ws;    // trailing white space count in postfix
    nfield =  0;

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
        sepc = *sepp;
        
        for (pre_ws = 0; *(p + pre_ws) == ' '; pre_ws++)
            ;
        for (post_ws = 0; *(sepp-post_ws) == ' '; post_ws++)
            ;
        *(sepp-post_ws) = '\0';               // terminate field
        *sepp = '\0'; // overwrite separator
        if (pre_ws > 0)
            memmove(p, p+pre_ws, (sepp-post_ws-p));       
        
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


#elif defined(LIB_4_1)

#include <stdlib.h>

typedef enum { false, true } bool;

enum { NOMEM = -2 };         

static char *line       = NULL;  
static char *sline      = NULL;  
static char **field     = NULL;  
static char fieldsep[]  = ",";

static int  maxline  = 0;     
static int  maxfield = 0;     
static int  nfield   = 0;

static bool is_line_split = false;

static int    endofline   (FILE *fin, int c);
static int    split       (void);
static char   *advquoted  (char *p);
static void   reset       (void);


// csvgetline: get one line, grow as needed
// sample input: "LU",86.25,"11/4/1998","2:19PM",+4.0625
char *csvgetline(FILE *fin)
{
    int i, c;
    char *newl, *news;

    is_line_split = false;

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
    return (c == EOF && i == 0) ? NULL : line;
}

// csvfield: return ptr to the n-th field
char *csvfield(int n)
{
    if (!is_line_split && split() == NOMEM) {
        reset();
        return NULL;
    }
    if (n < 0 || n >= nfield)
        return NULL;
    return field[n];
}

// csvnfield: return number of fields
int csvnfield(void)
{
    if (!is_line_split && split() == NOMEM) {
        reset();
        return NOMEM;
    }
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
    char *sepp;      // ptr to temp separator char
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
    is_line_split = true;
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
#elif defined(LIB_4_3)

#include <stdlib.h>

typedef enum { false, true } bool;

enum { NOMEM = -2, ALREADY_INITIALIZED = -3 };         

static char *line       = NULL;  
static char *sline      = NULL;  
static char **field     = NULL;  
static char fieldsep[]  = ",";

static int  maxline  = 0;     
static int  maxfield = 0;     
static int  nfield   = 0;

static bool is_initialized = false;

static int    endofline   (FILE *fin, int c);
static int    split       (void);
static char   *advquoted  (char *p);
static void   reset       (void);

int init(int n_line, int n_field)
{
    if (!is_initialized) {           // initialize data structures
        is_initialized = true;
        maxline = n_line ? n_line : 1;
        maxfield = n_field ? n_field : 1;
        line = (char *) malloc(maxline);
        sline = (char *) malloc(maxline);
        field = (char **) malloc(maxfield*sizeof(field[0]));
        if (line == NULL || sline == NULL || field == NULL) {
            reset();
            return NOMEM;
        }
        return 0;
    }
    return ALREADY_INITIALIZED;
}

// csvgetline: get one line, grow as needed
// sample input: "LU",86.25,"11/4/1998","2:19PM",+4.0625
char *csvgetline(FILE *fin)
{
    int i, c;
    char *newl, *news;

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
void reset(void)
{
    free(line);
    free(sline);
    free(field);
    line = NULL;
    sline = NULL;
    field = NULL;
    maxline = maxfield = nfield = 0;
}

// destroy: free up resources
void destroy(void)
{
    reset();
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
    char *sepp;      // ptr to temp separator char
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
#elif defined(LIB_4_4)
#define STRING  s
#define NUMBER  n
static char separator = '\n';

static int n_format(char *fmt)
{
    int n, i;
    if (*fmt != '%')
        return -1;
    for (i = 0; *(fmt + i) != '\0'; i++) {
        if (*(fmt + i) == '%' && i % 3 == 0) {
            n++;
            if (*(fmt + i + 1) != STRING || *(fmt + i + 1) != NUMBER)
                return -1;
        } else if (*(fmt + i) == '%' && i % 3 != 0) {
            return -1;
        }
    }
    return n;
}

char *csvformat(char **data, int n_field, char *fmt)
{
    int i, total_len;
    char *line, *p;

    if (fmt == NULL || n_field <= 0 || n_field != n_format(fmt))
        return NULL;

    total_len = 0;
    for (i = 0; i < n_field; i++)
        total_len += strlen(*(data + i));
    
    line = malloc(sizeof(char) * (total_len + (n_field * 3)));
    if (line == NULL)
        return NULL;

    for (i = 0; i < n_field; i++) { 
        if (*(fmt + (3*i)+1) == STRING) {
            *line++ = '"';
            for (p = *data; *p != '\0'; p++) 
                *line++ = *p;
            *line++ = '"';
        } else {
            for (p = *data; *p != '\0'; p++) 
                *line++ = *p;
        }   
        separator = *(fmt + (3*i) + 2);
        *line++ = separator ? separator : '\n';
    }    

    *line = '\0';
    return line;
}

#elif defined(LIB_4_8)
#include <stdlib.h>

enum { NOMEM = -2, ALREADY_INITIALIZED = -3 };         

struct CSV {
    FILE  *fin;
    int   maxline;     
    int   maxfield;     
    int   nfield;   
    char  *line;  
    char  *sline;  
    char  **field;  
    char  *fieldsep;
};

static int    endofline   (FILE *fin, int c);
static int    split       (CSV *csv);
static char   *advquoted  (char *p, char *field_sep);
static void   reset       (CSV *csv);

CSV *csvnew(FILE *fin, int n_lines, int n_field, char *field_sep)
{
    CSV *csv;
    csv = (CSV *) malloc(sizeof(CSV));
    csv->fin = fin;
    csv->maxline = n_lines ? n_lines : 512;
    csv->maxfield = n_field ? n_field : 32;
    csv->nfield = 0;
    csv->fieldsep = field_sep != NULL ? field_sep : ",";
    
    csv->line = NULL;
    csv->sline = NULL;
    csv->field = NULL;

    return csv;
}

// csvgetline: get one line, grow as needed
// sample input: "LU",86.25,"11/4/1998","2:19PM",+4.0625
char *csvgetline(CSV *csv)
{
    int i, c;
    char *newl, *news;

    if (csv->line == NULL) {           // allocate on first call
        csv->maxline = csv->maxfield = 1;
        csv->line = (char *) malloc(csv->maxline*sizeof(char));
        csv->sline = (char *) malloc(csv->maxline*sizeof(char));
        csv->field = (char **) malloc(csv->maxfield*sizeof(csv->field[0]));
        if (csv->line == NULL || csv->sline == NULL || csv->field == NULL) {
            reset(csv);
            return NULL;
        }
    }
    for (i=0; (c=getc(csv->fin))!=EOF && !endofline(csv->fin,c); i++) {
        if (i >= csv->maxline-1) {     // grow line
            csv->maxline *= 2;         // double current size
            newl = (char *) realloc(csv->line, csv->maxline);
            news = (char *) realloc(csv->sline, csv->maxline);
            if (newl == NULL || news == NULL) {
                reset(csv);
                return NULL;
            }
            csv->line = newl;
            csv->sline = news;
        }
        csv->line[i] = c;
    }
    csv->line[i] = '\0';
    if (split(csv) == NOMEM) {
        reset(csv);
        return NULL;
    }
    return (c == EOF && i == 0) ? NULL : csv->line;
}

// csvfield: return ptr to the n-th field
char *csvfield(CSV *csv, int n)
{
    if (n < 0 || n >= csv->nfield)
        return NULL;
    return csv->field[n];
}

// csvnfield: return number of fields
int csvnfield(CSV *csv)
{
    return csv->nfield;
}

// reset: set variables back to starting values
static void reset(CSV *csv)
{
    free(csv->line);
    free(csv->sline);
    free(csv->field);
    csv->line = NULL;
    csv->sline = NULL;
    csv->field = NULL;
    csv->maxline = csv->maxfield = csv->nfield = 0;
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
static int split(CSV *csv)
{
    char  *p, **newf;
    char  *sepp;      // ptr to separator char
    int   sepc;       // temp separator char
    csv->nfield =  0;

    if (csv->line[0] == '\0')
        return 0;

    strcpy(csv->sline, csv->line);
    p = csv->sline;

    do {
        if (csv->nfield >= csv->maxfield) {
            csv->maxfield *= 2;          // double current size
            newf = (char **) realloc(csv->field,
                        csv->maxfield * sizeof(csv->field[0]));
            if (newf == NULL)
                return NOMEM;
            csv->field = newf;
        }
        if (*p == '"')
            sepp = advquoted(++p, csv->fieldsep);    // skip initial quote
        else
            sepp = p + strcspn(p, csv->fieldsep);
        sepc = *sepp;
        
        for ( ; *p == ' '; p++)
            ;
        for ( ; *sepp == ' '; sepp--)
            ;
        *sepp = '\0'; // overwrite separator       
        
        csv->field[csv->nfield++] = p;
        p = sepp + 1;
    } while (sepc == ',');
    
    return csv->nfield;
}


// advquoted: quoted field; return ptr to next separator
static char *advquoted(char *p, char *field_sep)
{
    int i, j;

    for (i = j = 0; p[j] != '\0'; i++, j++) {
        if (p[j] == '"' && p[++j] != '"') {
            // copy up to next separator or \0
            int k = strcspn(p+j, field_sep);
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

#endif
