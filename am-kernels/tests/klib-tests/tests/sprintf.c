#include "trap.h"
#include <limits.h>

#define N 8

int data[] = {0, INT_MAX / 17, INT_MAX, INT_MIN, INT_MIN + 1,
              UINT_MAX / 17, INT_MAX / 17, UINT_MAX};
// 0 126322567 2147483647 -2147483648 -2147483647 252645135 126322567 -1
char *str[] = {
  "0",
  "126322567",
  "2147483647",
  "-2147483648",
  "-2147483647",
  "252645135",
  "126322567",
  "-1",
};

char ans[32][32];

int main() {

  for (int i = 0; i < N; i++) {
    sprintf(ans[i], "%d", data[i]);
  }
  /* for (int i = 0; i < N; i++) { */
  /*   printf("%s", ) */
  /* } */
  for (int i = 0; i < N; i++) {
    check(strcmp(str[i], ans[i]) == 0);
  }
  return 0;
}
