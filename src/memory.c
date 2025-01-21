#include <stddef.h>
#include <stdint.h>

int strcmp(const char* s1, const char* s2)
{
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void* memcpy(void* dest, const void* src, size_t n)
{
  uint8_t* pdest = (uint8_t*)dest;
  const uint8_t* psrc = (const uint8_t*)src;

  for (size_t i = 0; i < n; i++) {
    pdest[i] = psrc[i];
  }

  return dest;
}

void* memset(void* s, int c, size_t n)
{
  uint8_t* p = (uint8_t*)s;

  for (size_t i = 0; i < n; i++) {
    p[i] = (uint8_t)c;
  }

  return s;
}

void* memmove(void* dest, const void* src, size_t n)
{
  uint8_t* pdest = (uint8_t*)dest;
  const uint8_t* psrc = (const uint8_t*)src;

  if (src > dest) {
    for (size_t i = 0; i < n; i++) {
      pdest[i] = psrc[i];
    }
  } else if (src < dest) {
    for (size_t i = n; i > 0; i--) {
      pdest[i - 1] = psrc[i - 1];
    }
  }

  return dest;
}

int memcmp(const void* s1, const void* s2, size_t n)
{
  const uint8_t* p1 = (const uint8_t*)s1;
  const uint8_t* p2 = (const uint8_t*)s2;

  for (size_t i = 0; i < n; i++) {
    if (p1[i] != p2[i]) {
      return p1[i] < p2[i] ? -1 : 1;
    }
  }

  return 0;
}

size_t strlen(const char* str)
{
  const char* s;

  for (s = str; *s; ++s)
    ;
  return (s - str);
}

size_t strnlen(const char* str, size_t maxlen)
{
  size_t len = 0;
  while (str[len] && len <= maxlen)
    len++;
  return len;
}

void* memchr(const void* buf, int c, size_t n)
{
  unsigned char* p = (unsigned char*)buf;
  unsigned char* end = p + n;
  while (p != end) {
    if (*p == c) {
      return p;
    }
    ++p;
  }
  return 0;
}

char* strrchr(const char* s, int c)
{
  unsigned char* p = (unsigned char*)s;
  unsigned char* last = 0;
  while (*p != 0) {
    if (*p == c) {
      last = p;
    }
    ++p;
  }
  return (char*)last;
}
