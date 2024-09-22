#include <stdio.h>
#define MAX_COUNT 1000

int main(int argc, char **argv) {
  int count = 0;
  int n = 1;
  while (count++ < MAX_COUNT) {
    n = 1;
    printf("%d, %d\n", n++, n++);
  }
  return 0;
}
