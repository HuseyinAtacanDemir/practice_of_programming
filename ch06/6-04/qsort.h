#ifndef QSORT_GENERIC_H
#define QSORT_GENERIC_H

#include <string.h>
#include <stdlib.h>

void qsort_generic(void *arr, int len, int elem_size, 
                    int (*cmp)(const void*, const void *));

#endif
