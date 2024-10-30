#ifndef QSORT_GENERIC_H
#define QSORT_GENERIC_H

#include <string.h>
#include <stdlib.h>

#define QSORT_ASC   0
#define QSORT_DESC  1

void qsort_generic  (void *arr, int n, int elemsize, int (*cmp)(void*, void *), 
                      int order);

#endif
