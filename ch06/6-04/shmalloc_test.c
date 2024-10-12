#include <stdio.h>
#include <stdlib.h>

#include "shmalloc.h"

int main(void)
{

    int n_a = 2000000;
    int n_b = 2000000;

    int *a = (int *) shmalloc(sizeof(int) * n_a);

    for (int i = 0; i < n_a; i++) {
        *(a+i) = i;
        printf("%d ", *(a+i));
    }
    printf("\n");
    int *b = (int *) shmalloc(sizeof(int) * n_b);
    for (int i = 0; i < n_b; i++) {
        *(b+i) = i;
        printf("%d ", *(b+i));
    }
    printf("\nshfree(a): %d\n", shfree(a));
    printf("\nshfree(b): %d\n", shfree(b));
    printf("\nshfree_all: %d\n", shfree_all());
    return 0;
}
