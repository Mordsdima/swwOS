#ifndef _STRING_H
#define _STRING_H

char* strchr(const char* str, int ch);
long int strtol(const char* str, char** endptr, int base);
char *strcpy(char *dst, const char *src);
int strcmp(const char* s1, const char* s2);
size_t strnlen(const char *s, size_t maxlen);
int strncmp( const char * s1, const char * s2, size_t n );

void *memset(void *s, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
static inline long strlen(const char* string) {
    long i;

    for (i = 0; string[i] != '\0'; ++i);

    return i;
}
#endif
