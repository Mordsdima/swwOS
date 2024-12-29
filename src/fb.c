#include <font.h>
#include <limine.h>
#include <stddef.h>
#define SSFN_CONSOLEBITMAP_TRUECOLOR
#include <ssfn.h>

void fb_setup(struct limine_framebuffer* framebuffer)
{
  ssfn_src = (ssfn_font_t*)&font_ssfn;
  ssfn_dst.ptr = framebuffer->address;
  ssfn_dst.w = framebuffer->width;
  ssfn_dst.h = framebuffer->height;
  ssfn_dst.p = framebuffer->pitch;
  ssfn_dst.fg = 0xFFFFFFFF;
  ssfn_dst.bg = 0;
}

void raw_print(char* str)
{
  int n = 0;
  while (str[n]) {
    ssfn_putc(str[n]);
    n++;
  }
}
