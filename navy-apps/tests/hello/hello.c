#include <unistd.h>
#include <stdio.h>

#define SYS_yield 1
extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);
int main() {
  write(1, "Hello World!\n", 13); // 将会调用 _exit 退出程序
  write(1, "Hello World!\n", 13);
  _syscall_(SYS_yield, 0, 0, 0);
  _syscall_(SYS_yield, 0, 0, 0);
  printf("Hello World from Navy-apps!\n");
  int i = 2;
  volatile int j = 0;
  while (1) {
    j ++;
    if (j == 10000) {
      printf("Hello World from Navy-apps for the %dth time!\n", i ++);
      j = 0;
    }
  }
  return 0;
}
