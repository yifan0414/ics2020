#include <unistd.h>
#include <stdio.h>

#define SYS_yield 1
extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);
int main() {
  printf("Hello World from Navy-apps!\n");
  int i = 2;
  volatile int j = 0;
  while (1) {
    j ++;
    if (j == 1000) {
      printf("Hello World from Navy-apps for the %dth time!\n", i ++);
      printf("Hello World from Navy-apps for the %dth time!\n", i ++);
      break;
    }
  }
  return 0;
}
