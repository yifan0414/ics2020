#include <am.h>
#include <klib-macros.h>
#include <klib.h>
#include <limits.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int puts(const char *s) {
  while (*s != '\0') {
    putch(*s++);
  }
  // putch('\n');

  return 1;
}

int vprintf(const char *fmt, va_list ap) {
  char buffer[256];
  int retval = vsprintf(buffer, fmt, ap);
  if (retval >= 0) {
    puts(buffer);
  }
  return retval;
}

int printf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int written = vprintf(fmt, args);
  va_end(args);
  return written;
}

void reverse(char *begin, char *end) {
  char *l = begin;
  char *r = end;
  while (l < r) {
    char temp = *l;
    *l = *r;
    *r = temp;
    l++;
    r--;
  }
}

char *itoa(int value, char *str) {
  char *start = str;
  int is_negative = value < 0;
  if (value == INT_MIN) {
    strcpy(str, "-2147483648");
    return str;
  } else if (is_negative) {
    value = -value;
  }
  do {
    *str++ = '0' + value % 10;
    value /= 10;
  } while (value > 0);

  if (is_negative) {
    *str++ = '-';
  }
  *str = '\0';
  reverse(start, str - 1);
  return start;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char *str;
  char *string_val;
  int int_val;
  int written = 0;

  for (str = (char *)fmt; *str != '\0'; str++) {
    if (*str != '%') {
      *out++ = *str;
      written++;
      continue;
    }

    str++;

    switch (*str) {
    case 'd':
      int_val = va_arg(ap, int);
      char buffer[32];
      itoa(int_val, buffer);
      char *p = buffer;
      while (*p != '\0') {
        *out++ = *p++;
        written++;
      }
      break;
    case 's':
      string_val = va_arg(ap, char *);
      while (*string_val != '\0') {
        *out++ = *string_val++;
        written++;
      }
      break;
    default:
      break;
    }
  }
  *out = '\0';

  return written;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int retval = vsprintf(out, fmt, ap);
  va_end(ap);
  return retval;
}

int snprintf(char *str, size_t n, const char *fmt, ...) { return 0; }

int vsnprintf(char *str, size_t n, const char *fmt, va_list ap) { return 0; }

#endif
