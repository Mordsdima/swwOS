#pragma once

#define VMM_FLAG_WRITE   ((uint64_t)1 << 0)
#define VMM_FLAG_NOEXEC  ((uint64_t)1 << 1)
#define VMM_FLAG_FB      ((uint64_t)1 << 2)

#define VMM_MAX_LEVEL 3

#define PAGING_MODE_AARCH64_4LVL 0
#define PAGING_MODE_AARCH64_5LVL 1

#define PAGING_MODE_MIN PAGING_MODE_AARCH64_4LVL
#define PAGING_MODE_MAX PAGING_MODE_AARCH64_5LVL

#define paging_mode_va_bits(mode) ((mode) ? 53 : 49)

static inline uint64_t paging_mode_higher_half(int paging_mode) {
    if (paging_mode == PAGING_MODE_AARCH64_5LVL) {
        return 0xffe0000000000000;
    } else {
        return 0xffff000000000000;
    }
}

typedef struct {
    int   levels;
    void *top_level[2];
} pagemap_t;

enum page_size {
    Size4KiB,
    Size2MiB,
    Size1GiB
};

void vmm_assert_4k_pages(void);
pagemap_t new_pagemap(int lv);
void map_page(pagemap_t pagemap, uint64_t virt_addr, uint64_t phys_addr, uint64_t flags, enum page_size page_size);