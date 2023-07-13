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
  return 0;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  return 0;
}

void* memset(void* v,int c,size_t n) {
  unsigned char *p = v;
  while (n--)
    *p++ = (unsigned char)c;
  return v;
}

void* memmove(void* dst,const void* src,size_t n) {
  return NULL;
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
