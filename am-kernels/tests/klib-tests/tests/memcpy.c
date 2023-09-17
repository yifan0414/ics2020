#include "trap.h"

char a[] = "1234567890";
char b[] = "0000000000";

int main() {
  // WARN: 没有测试重叠情况
  memcpy(a, b, 4);
  check(strcmp(a, "0000567890") == 0);
  return 0;
}
