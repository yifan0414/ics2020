#include "trap.h"

#define N 10
char* a[] = {"1", "12", "123", "1234", "12345", "123456", "1234567", "12345678", "123456789", "1234567890"};

char* b[] = {"1234567890", "234567890", "34567890", "4567890", "567890", "67890", "7890", "890", "90", "0"};

void testdgs() {
  for (int i = 0; i < N; i++) {
    char s[20] = "1234567890";
    char *d = &s[i];
    memmove(d, s, N);
    check(strncmp(s, a[i], i) == 0);
    // printf("d = %p, s = %p\n", d, s);
    // printf("strncmp = %d\n", strncmp(s, a[i], i));
    // printf("str(s) = %s\n", s);
    // printf("str(d) = %s\n", d);
    // printf("str(a[%d]) = %s\n", i, a[i]);
    check(strcmp(d, "1234567890") == 0);
    check(strcmp(&s[i], "1234567890") == 0);
  }
}

void testsgd() {
  for (int i = 0; i < N; i++) {
    char d[20] = {'\0'};
    char *s = &d[N - i];
    strcpy(s, "1234567890");
    memmove(d, s, N);
    // printf("str(s) = %s\n", s);
    // printf("str(d) = %s\n", d);
    check(strncmp(d, "1234567890", 10) == 0);
    // printf("str(b[%d]) = %s\n", i, b[i]);
    // printf("str(s + %d) = %s\n", i, s + i);
    check(strcmp(s + i, b[i]) == 0);
  }
}

int main() {
  testdgs();
  testsgd();
  return 0;
}
