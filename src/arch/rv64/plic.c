#include <fb.h>
#include <fdt/libfdt.h>
#include <interrupt_controller.h>
#include <liballoc.h>
#include <stdbool.h>
#include <stdint.h>

extern uint64_t hhdm_off;

#define MAP_TO_HHDM(addr) (hhdm_off + (uint64_t)(addr))
#define PLIC_CONTEXT_BASE 0x200000

uint32_t plic_claim(struct interrupt_controller* ic)
{
  return *(uint32_t*)&((uint8_t*)ic->data)[0x200004];
}

void plic_threshold(struct interrupt_controller* ic, uint8_t threshold)
{
  volatile void* plic_thresh;

  plic_thresh = (char*)ic->data + PLIC_CONTEXT_BASE;

  *(uint64_t*)plic_thresh = threshold;
}

struct interrupt_controller* init_plic(void* dtb)
{
  int32_t plic = fdt_node_offset_by_compatible(dtb, -1, "sifive,plic-1.0.0");
  while (plic < 0) {
    plic = fdt_node_offset_by_compatible(dtb, plic, "sifive,plic-1.0.0");
  }
  if (plic == -FDT_ERR_NOTFOUND)
    return NULL;
  int len;
  const fdt64_t* reg = fdt_getprop(dtb, plic, "reg", &len);
  uint64_t* plic_base = (uint64_t*)(*reg);
  struct interrupt_controller* ic = kmalloc(sizeof(struct interrupt_controller));
  ic->data = (void*)map_mmio((uint64_t)plic_base);
  ic->claim = &plic_claim;
  ic->threshold = &plic_threshold;
  return ic;
}
