#pragma once

#include <stddef.h>

int strcmp(const char* s1, const char* s2);
void* memcpy(void* dest, const void* src, size_t n);
void* memset(void* s, int c, size_t n);
void* memmove(void* dest, const void* src, size_t n);
int memcmp(const void* s1, const void* s2, size_t n);
size_t strlen(const char* str);
char* strrchr(const char* s, int c);
void* memchr(const void* buf, int c, size_t n);
size_t strnlen(const char* str, size_t maxlen);
