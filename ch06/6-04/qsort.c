#include "qsort.h"

void swap(void *arr, int elem_size, int i, int j)
{
    void *a = ((char *)arr) + (i*elem_size);
    void *b = ((char *)arr) + (j*elem_size);
    char tmp[elem_size];

    memcpy(tmp, a, elem_size);
    memcpy(a, b, elem_size);
    memcpy(b, tmp, elem_size);
}

void qsort_generic(void *arr, int len, int elem_size, 
                    int (*cmp)(const void *, const void *), int order)
{
    void *bigger, *cur;
    int i, last_small, sort_order;
    
    sort_order = order ? order : QSORT_ASC;

    if (len <= 1 || arr == NULL)
        return;

    swap(arr, elem_size, 0, rand() % len);
    
    last_small = 0;
    for (i = 1; i < len; i++) {
        cur = ((char *)arr)+(i*elem_size);
        if ((sort_order == QSORT_ASC ? cmp(cur, arr) < 0 : cmp(cur, arr) > 0))
            swap(arr, elem_size, i, ++last_small);
    }

    swap(arr, elem_size, 0, last_small);
    
    qsort_generic(arr, last_small, elem_size, cmp, sort_order);

    bigger = ((char *)arr) + ((last_small + 1) * elem_size);
    qsort_generic(bigger, (len-last_small-1), elem_size, cmp, sort_order);
}
