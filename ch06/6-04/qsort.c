#include "qsort.h"

void swap(void *arr, int elemsize, int i, int j)
{
    void *a = ((char *)arr) + (i*elemsize);
    void *b = ((char *)arr) + (j*elemsize);
    char tmp[elemsize];

    memcpy(tmp, a, elemsize);
    memcpy(a, b, elemsize);
    memcpy(b, tmp, elemsize);
}

void qsort_generic(void *arr, int len, int elemsize, int (*cmp)(void*, void*), 
                    int order)
{
    void *bigger, *cur;
    int i, last_small, sort_order;
    
    sort_order = order ? order : QSORT_ASC;

    if (len <= 1 || arr == NULL)
        return;

    swap(arr, elemsize, 0, rand() % len);
    
    last_small = 0;
    for (i = 1; i < len; i++) {
        cur = ((char *)arr)+(i*elemsize);
        if ((sort_order == QSORT_ASC ? cmp(cur, arr) < 0 : cmp(cur, arr) > 0))
            swap(arr, elemsize, i, ++last_small);
    }

    swap(arr, elemsize, 0, last_small);
    
    qsort_generic(arr, last_small, elemsize, cmp, sort_order);

    bigger = ((char *)arr) + ((last_small + 1) * elemsize);
    qsort_generic(bigger, (len-last_small-1), elemsize, cmp, sort_order);
}
