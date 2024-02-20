#include <stdint.h>
#include <stddef.h>

void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;
 
    for (size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }
 
    return dest;
}
 
void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;
 
    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }
 
    return s;
}
 
void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;
 
    if (src > dest) {
        for (size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if (src < dest) {
        for (size_t i = n; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }
 
    return dest;
}
 
int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;
 
    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }
 
    return 0;
}

const char* strchr(const char* str, int ch) {
    while (*str != '\0' && *str != ch)
        str++;

    if (*str == ch)
        return str + 1;  // return pointer to the character after the found character
    else
        return NULL;
}

char * strcpy(char *dst, const char *src)
{
    while (*src != '\0')
    {
        *dst = *src;
        dst++;
        src++;
    }
    
    *dst = '\0';

    return dst;
}

long int strtol(const char* str, char** endptr, int base) {
    long int result = 0;
    int sign = 1;

    // Пропустить начальные пробелы
    while (*str == ' ')
        str++;

    // Обработка знака
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    // Определение основания системы счисления
    if (base == 0) {
        if (*str == '0') {
            str++;
            if (*str == 'x' || *str == 'X') {
                base = 16;
                str++;
            } else {
                base = 8;
            }
        } else {
            base = 10;
        }
    }

    // Преобразование строки в число
    while (*str != '\0') {
        int digit;
        if (*str >= '0' && *str <= '9') {
            digit = *str - '0';
        } else if (*str >= 'a' && *str <= 'f') {
            digit = *str - 'a' + 10;
        } else if (*str >= 'A' && *str <= 'F') {
            digit = *str - 'A' + 10;
        } else {
            break;
        }

        if (digit >= base)
            break;

        result = result * base + digit;
        str++;
    }

    if (endptr != NULL)
        *endptr = (char*)str;

    return result * sign;
}

size_t strnlen(const char *s, size_t maxlen) {
	size_t len;
	for (len = 0; len < maxlen; len++, s++)
		if (!*s) break;
	return (len);
}

int strncmp( const char * s1, const char * s2, size_t n ) {
    while ( n && *s1 && ( *s1 == *s2 ) )
    {
        ++s1; ++s2; --n;
    }
    if ( n == 0 )
        return 0;
    else
        return ( *(unsigned char *)s1 - *(unsigned char *)s2 );
}

int strcmp(const char* s1, const char* s2) {
    while(*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}