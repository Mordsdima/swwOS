#pragma once
#include <limine.h>

void fb_setup(struct limine_framebuffer* framebuffer);
void raw_print(const char* str);
void raw_putc(const char c);
