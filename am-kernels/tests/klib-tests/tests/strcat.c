#include "trap.h"


char a[20] = "1234567890";

char b[] = "abcdefg";

int main() {
  check(strlen(a) == 10);
  strcat(a, b);
  check(strcmp(a, "1234567890abcdefg") == 0);
  return 0;
}
