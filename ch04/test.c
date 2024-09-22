#include "csv.h"
#include <stdio.h>

// csvtest main: test csvgetline function
#if defined(PROTO)
int main(void)
{
    int i, nf;

    while ((nf = csvgetline(stdin)) != -1)
        for (i = 0; i < nf; i++)
            printf("field[%d] = '%s'\n", i, field[i]);
    return 0;
}

#elif defined(LIB_4_3)
int main(void)
{
    int i;
    char *line;
    // with prior knowledge of exact field count 
    // but only heuristic knowledge of  max field length
    init(256, 8);
    while ((line = csvgetline(stdin)) != NULL) {
        printf("line: '%s'\n", line);
        for (i = 0; i < csvnfield(); i++)
            printf("field[%d] = '%s'\n", i, csvfield(i));
    }
    destroy();
    return 0;
}

#else
int main(void)
{
    int i;
    char *line;

    while ((line = csvgetline(stdin)) != NULL) {
        printf("line: '%s'\n", line);
        for (i = 0; i < csvnfield(); i++)
            printf("field[%d] = '%s'\n", i, csvfield(i));
    }
    return 0;
}

#endif
