#include "interrupt_controller.h"
#include "limine.h"
#include <arch/rv64/plic.h>
#include <fb.h>
#include <pmm.h>

__attribute__((used, section(".limine_requests_start"))) static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end"))) static volatile LIMINE_REQUESTS_END_MARKER;

__attribute__((used, section(".limine_requests"))) static volatile struct limine_dtb_request dtb_req = {
  .id = LIMINE_DTB_REQUEST,
  .revision = 0
};

struct interrupt_controller* ic;

void kmain()
{
  // some things already inited, so lets just continue initing our system!
  raw_print("Hello! Thats message from a kmain! :D\n");
  init_pmm();
  raw_print("PMM is now initialized!\n");
  if (!dtb_req.response) {
    raw_print("No dtb found.");
    goto after_plic;
  }
  ic = init_plic(dtb_req.response->dtb_ptr);
  if (!ic) {
    raw_print("PLIC is not found.\n");
    goto after_plic;
  }
  ic->threshold(ic, 0);
  raw_print("PLIC is now initialized!\n");
after_plic:
  raw_print("\nFor now its the end of kernel; thanks for watching!");
  for (;;)
    ;
}
