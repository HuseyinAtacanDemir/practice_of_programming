#include <stdio.h>
#include <stdlib.h>

void swap(int[], int, int);
void qsort_custom(int[], int);

int main(int argc, char **argv)
{
    int i, n;
    int arr[] = {
        3,
        9,
        12,
        0,
        -3,
        52,
        -94,
        15,
        7,
        90
    };
    
    n = (sizeof(arr)/sizeof(arr[0]));
    qsort_custom(arr, n);

    for (i = 0; i < n; i++)
        printf("%d\n", arr[i]);

    return 0;

}

          void swap(int v[], int i, int j)
          {
              int temp = v[i];
              v[i] = v[j];
              v[j] = temp;
          }

          // qsort: Iterative qsort using a stack to simulate
          // what the call stack does during recursion
          void qsort_custom(int v[], int n)
          {
              int i, start, end, last, top, stack[n];

              if (n <= 1)
                  return;

              start = 0;
              end   = n-1;

              top = -1;
              stack[++top] = start;
              stack[++top] = end;

              while (top > 0) {
                  end   = stack[top--];
                  start = stack[top--];
                  last  = start;

                  // pick a pivot and place it at the start idx
                  swap(v, start, start + (rand()%(end-start+1)));        

                  for (i = start+1; i < end+1; i++)
                      if (v[i] < v[start])
                          swap(v, i, ++last);

                  // restore pivot to partition idx
                  swap(v, start, last);

                  if ((last-1) - start >= 1) {
                      stack[++top] = start;
                      stack[++top] = last - 1;
                  }

                  if (end - (last+1) >= 1) {
                      stack[++top] = last + 1;
                      stack[++top] = end;
                  }
              }
          }
