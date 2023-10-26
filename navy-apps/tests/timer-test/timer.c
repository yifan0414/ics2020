#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

int main() {
  struct timeval start, now;
  gettimeofday(&start, NULL);

  while (1) {
    gettimeofday(&now, NULL);
    int elapsed_time =
        (now.tv_sec - start.tv_sec) * 1000000 + (now.tv_usec - start.tv_usec);
    if (elapsed_time >= 1000000) {
      printf("1/2 second has passed.\n");
      printf("%ld %ld\n", now.tv_sec, now.tv_usec);
      start = now;
    }
    // printf("%d\n", elapsed_time);
    // if (elapsed_time >= 500000) {
    //   break;
    // }
  }

  return 0;
}
