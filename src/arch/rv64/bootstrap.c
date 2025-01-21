#include <fb.h>
#include <limine_requests.h>
#include <stdbool.h>
#include <stddef.h>

extern void kmain();

__attribute__((used, section(".limine_requests")))
LIMINE_BASE_REVISION(3);

__attribute__((used, section(".limine_requests"))) static volatile struct limine_framebuffer_request framebuffer_request = {
  .id = LIMINE_FRAMEBUFFER_REQUEST,
  .revision = 0
};

void kbootstrap()
{
  if (LIMINE_BASE_REVISION_SUPPORTED == false)
    for (;;)
      ;

  if (framebuffer_request.response == NULL
      || framebuffer_request.response->framebuffer_count < 1) {
    for (;;)
      ;
  }

  struct limine_framebuffer* framebuffer = framebuffer_request.response->framebuffers[0];

  fb_setup(framebuffer);

  raw_print("RISC-V platform hello! :D\n");

  kmain();
}
