// read src/arch/*/vmm.c
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#if defined (__riscv)

// We use fake flags here because these don't properly map onto the
// RISC-V flags.
#define VMM_FLAG_WRITE   ((uint64_t)1 << 0)
#define VMM_FLAG_NOEXEC  ((uint64_t)1 << 1)
#define VMM_FLAG_FB      ((uint64_t)1 << 2)

#define VMM_MAX_LEVEL 5

#define PAGING_MODE_RISCV_SV39 8
#define PAGING_MODE_RISCV_SV48 9
#define PAGING_MODE_RISCV_SV57 10

#define PAGING_MODE_MIN PAGING_MODE_RISCV_SV39
#define PAGING_MODE_MAX PAGING_MODE_RISCV_SV57

int paging_mode_va_bits(int paging_mode);

enum page_size {
    Size4KiB,
    Size2MiB,
    Size1GiB,
    Size512GiB,
    Size256TiB
};

typedef struct {
    enum page_size max_page_size;
    int            paging_mode;
    void          *top_level;
} pagemap_t;

uint64_t paging_mode_higher_half(int paging_mode);
int vmm_max_paging_mode(void);
pagemap_t new_pagemap(int paging_mode);
void map_page(pagemap_t pagemap, uint64_t virt_addr, uint64_t phys_addr, uint64_t flags, enum page_size page_size);

#endif

void map_pages(pagemap_t pagemap, uint64_t virt, uint64_t phys, uint64_t flags, uint64_t count);
