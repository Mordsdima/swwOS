#include <limine.h>
#include <stdarg.h>
#include <stddef.h>
#include <bootstub.h>

#pragma once

int terminal_init(fb_info_t *fb);
int print(const char* string, size_t len);
int tprintf(const char* __restrict, ...);
int tvsprintf(const char* restrict format, va_list parameters);
void clear();