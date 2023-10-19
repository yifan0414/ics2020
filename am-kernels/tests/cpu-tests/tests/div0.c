#include "trap.h"

#define N 10
int a[N];

int main() {
  int i = 10;
  int j = 0;
  int a = i / j;
  i = a;
  printf("%d\n", i);

  return 0;
}
