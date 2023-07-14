#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  return 0;
}

char *strcpy(char* dst,const char* src) {
  int i = 0;
  do {
    dst[i] = src[i];
  } while(src[i++] != '\0');
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  return NULL;
}

char* strcat(char* dst, const char* src) {
  return NULL;
}

int strcmp(const char* s1, const char* s2) {
  while(*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  while (n && *s1 && (*s1 == *s2)) {
      s1++;
      s2++;
      n--;
  }
  if (n == 0) {
      return 0;
  } else {
      return *(unsigned char*)s1 - *(unsigned char*)s2;
  }
}

void* memset(void* v,int c,size_t n) {
  unsigned char *p = v;
  while (n--)
    *p++ = (unsigned char)c;
  return v;
}

void* memmove(void* dst,const void* src,size_t n) {
  char *d = (char*) dst;
  const char *s = (char*) src;
  if (d > s) {
    while (n--) {
      d[n] = s[n];
    }
  } else {
    while (n--) {
      *d++ = *s++;
    }
  }
  return dst;
}

void* memcpy(void* out, const void* in, size_t n) {
  char* dest = (char*) out;
  const char* src = (const char*) in;
  for (int i = 0; i < n; i++) {
    dest[i] = src[i];
  }
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n) {
  return 0;
}

#endif
