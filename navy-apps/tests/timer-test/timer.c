#include <NDL.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

int main() {
  NDL_Init(0);
  int32_t start, now;
  start = NDL_GetTicks();

  while (1) {
    now = NDL_GetTicks();
    int elapsed_time = now - start;
    if (elapsed_time >= 500) {
      printf("1/2 second has passed.\n");
      start = now;
    }
  }
  NDL_Quit();

  return 0;
}
