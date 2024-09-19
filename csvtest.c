#include <stdio.h>
#include "csv.h"
// csvtest main: test csvgetline function
int main(void)
{
    int i, nf;

    while ((nf = csvgetline(stdin)) != -1)
        for (i = 0; i < nf; i++)
            printf("field[%d] = '%s'\n", i, field[i]);
    return 0;
}
