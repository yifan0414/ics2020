#include "trap.h"

int sub(int a, int b) {
  int c = a - b;
  return c;
}

int main() {
  check(sub(1 - 2) == -1);

	return 0;
}

