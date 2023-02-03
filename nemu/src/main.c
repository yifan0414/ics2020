#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
void init_monitor(int, char *[]);
void engine_start();
int is_exit_status_bad();
uint32_t expr(char *, bool *);

char buf[65535];

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  init_monitor(argc, argv);

  /* Start engine. */
  // engine_start();
  FILE *fp = fopen("./tools/gen-expr/res.txt", "r");
  int all_test = 0;
  int correct_test = 0;
  while (feof(fp) == 0) {
    uint32_t res = 0;
    memset(buf, 0, sizeof(buf));
    int retval = fscanf(fp, "%u %[^\n] ", &res, buf);
    if (retval != 2) {
      puts("read error\n");
    } else {
      all_test ++;
      bool success = true;
      uint32_t expr_ret = expr(buf, &success);
      if (res != expr_ret) {
        puts("Not match\n");
        printf("%d %s\n", res, buf);
      } else {
        correct_test ++;
      }
    }
  }
  printf("PASS CASE: %d/%d\n", correct_test, all_test);

  return is_exit_status_bad();
}
