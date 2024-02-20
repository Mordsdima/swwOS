#include <limine.h>
#include <stdarg.h>

#ifndef _TERMINAL_H_
#define _TERMINAL_H_

int terminal_init(struct limine_framebuffer *fb);
int print(const char* string, size_t len);
int tprintf(const char* __restrict, ...);
int tvsprintf(const char* restrict format, va_list parameters);
void clear();

#endif