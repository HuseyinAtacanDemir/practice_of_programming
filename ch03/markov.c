#include <stdlib.h>
#include <stdio.h>
#include <string.h>


enum {
    NPREF       = 2,      // number of prefix words
    NHASH       = 4093,   // size of state hash table arr
    MAXGEN      = 10000,  // maximum number of words generated
    MULTIPLIER  = 31
};
char NONWORD[] = "\n";

typedef struct State State;
typedef struct Suffix Suffix;
struct State { // prefix suffix list
    char    *pref[NPREF];   // prefix words
    Suffix  *suf;           // list of suffixes
    State   *next;          // next in hash table
};

struct Suffix { // list of suffixes
    char    *word;
    Suffix  *next;
};

State *statetab[NHASH];

unsigned int hash(char *s[NPREF])
{
    unsigned int h;
    unsigned char *p;
    int i;

    h = 0;
    for (i = 0; i < NPREF; i++)
        for (p = (unsigned char *) s[i]; *p != '\0'; p++)
            h = MULTIPLIER * h + *p;
    return h % NHASH;
}

State *lookup(char *prefix[NPREF], int create)
{
    int i, h;
    State *sp;
    
    h = hash(prefix);
    for (sp = statetab[h]; sp != NULL; sp = sp->next) {
        for (i = 0; i < NPREF; i++)
            if (strcmp(prefix[i], sp->pref[i]) != 0)
                break;
        if (i == NPREF)
            return sp;
    }
    if (create) {
        sp = (State *) malloc(sizeof(State));
        for (i = 0; i < NPREF; i++)
            sp->pref[i] = prefix[i];
        sp->suf = NULL;
        sp->next = statetab[h];
        statetab[h] = sp;
    }
    return sp;
}

void addsuffix(State *sp, char *suffix)
{
    Suffix *suf;

    suf = (Suffix *) malloc(sizeof(Suffix));
    suf->word = suffix;
    suf->next = sp->suf;
    sp->suf = suf;
}

void add(char *prefix[NPREF], char *suffix)
{
    State *sp;

    sp = lookup(prefix, 1);
    addsuffix(sp, suffix);
    memmove(prefix, prefix+1, (NPREF-1)*sizeof(prefix[0]));
    prefix[NPREF-1] = suffix;
}

void build(char *prefix[NPREF], FILE *f)
{
    char buf[100], fmt[10];

    sprintf(fmt, "%%%ds", (int) sizeof(buf)-1);
    while (fscanf(f, fmt, buf) != EOF)
        add(prefix, strdup(buf));
}

void generate(int nwords)
{
    State *sp;
    Suffix *suf;
    char *prefix[NPREF], *w;
    int i, nmatch;

    for (i = 0; i < NPREF; i++)
        prefix[i] = NONWORD;

    for (i = 0; i < nwords; i++) {
        sp = lookup(prefix, 0);
        nmatch = 0;
        for (suf = sp->suf; suf != NULL; suf = suf->next)
            if (rand() % ++nmatch == 0)
                w = suf->word;
        if (strcmp(w, NONWORD) == 0)
            break;
        printf("%s\n", w);
        memmove(prefix, prefix+1, (NPREF-1)*sizeof(prefix[0]));
        prefix[NPREF-1] = w;
    }
}

int main(void)
{
    int i, nwords = MAXGEN;
    char *prefix[NPREF];
    
    for (i = 0; i < NPREF; i++)
        prefix[i] = NONWORD;
    build(prefix, stdin);
    add(prefix, NONWORD);
    generate(nwords);
    return 0;
}
