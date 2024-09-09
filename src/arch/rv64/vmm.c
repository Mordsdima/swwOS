#if defined (__riscv)
#include <vmm.h>
#include <stdint.h>
#include <stddef.h>
#include <liballoc.h>

#define PT_SIZE ((uint64_t)0x1000)
typedef uint64_t pt_entry_t;


pagemap_t new_pagemap(int paging_mode) {
    pagemap_t pagemap;
    pagemap.paging_mode   = paging_mode;
    pagemap.max_page_size = paging_mode - 6;
    pagemap.top_level     = kmalloc(PT_SIZE);

    return pagemap;
}

void install_pagemap(pagemap_t pagemap) {
    (void)pagemap;
}

void map_page(pagemap_t pagemap, uint64_t virt_addr, uint64_t phys_addr, uint64_t flags, enum page_size pg_size) {
    (void)pagemap;
    (void)virt_addr;
    (void)phys_addr;
    (void)flags;
    (void)pg_size;
}

#endif