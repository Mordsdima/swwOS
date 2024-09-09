#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <vmm.h>
#include <pmm.h>
#include <utils.h>
#include <panic.h>
#include <liballoc.h>

static uint64_t page_sizes[5] = {
    0x1000,
    0x200000,
    0x40000000,
    0x8000000000,
    0x1000000000000,
};

#define PT_SIZE ((uint64_t)0x1000)
typedef uint64_t pt_entry_t;

static pt_entry_t *get_next_level(pagemap_t pagemap, pt_entry_t *current_level,
                                  uint64_t virt, enum page_size desired_sz,
                                  size_t level_idx, size_t entry);

#define PT_FLAG_VALID    ((uint64_t)1 << 0)
#define PT_FLAG_TABLE    ((uint64_t)1 << 1)
#define PT_FLAG_4K_PAGE  ((uint64_t)1 << 1)
#define PT_FLAG_BLOCK    ((uint64_t)0 << 1)
#define PT_FLAG_USER     ((uint64_t)1 << 6)
#define PT_FLAG_READONLY ((uint64_t)1 << 7)
#define PT_FLAG_INNER_SH ((uint64_t)3 << 8)
#define PT_FLAG_ACCESS   ((uint64_t)1 << 10)
#define PT_FLAG_XN       ((uint64_t)1 << 54)
#define PT_FLAG_WB       ((uint64_t)0 << 2)
#define PT_FLAG_FB       ((uint64_t)1 << 2)
#define PT_PADDR_MASK    ((uint64_t)0x0000FFFFFFFFF000)

#define PT_TABLE_FLAGS   (PT_FLAG_VALID | PT_FLAG_TABLE)

#define PT_IS_TABLE(x) (((x) & (PT_FLAG_VALID | PT_FLAG_TABLE)) == (PT_FLAG_VALID | PT_FLAG_TABLE))
#define PT_IS_LARGE(x) (((x) & (PT_FLAG_VALID | PT_FLAG_TABLE)) == PT_FLAG_VALID)
#define PT_TO_VMM_FLAGS(x) (pt_to_vmm_flags_internal(x))

#define pte_new(addr, flags)    ((pt_entry_t)(addr) | (flags))
#define pte_addr(pte)           ((pte) & PT_PADDR_MASK)

static uint64_t pt_to_vmm_flags_internal(pt_entry_t entry) {
    uint64_t flags = 0;

    if (!(entry & PT_FLAG_READONLY))
        flags |= VMM_FLAG_WRITE;
    if (entry & PT_FLAG_XN)
        flags |= VMM_FLAG_NOEXEC;
    if (entry & PT_FLAG_FB)
        flags |= VMM_FLAG_FB;

    return flags;
}

pagemap_t new_pagemap(int paging_mode) {
    pagemap_t pagemap;
    pagemap.levels       = paging_mode == PAGING_MODE_AARCH64_5LVL ? 5 : 4;
    pagemap.top_level[0] = kmalloc(PT_SIZE);
    pagemap.top_level[1] = kmalloc(PT_SIZE);
    return pagemap;
}

void install_pagemap(pagemap_t pagemap) {
    // Lets install pagemap into TTBR0/TTBR1
    WRITE_SYSREG_64(ttbr0_el1, (uint64_t)pagemap.top_level[0]);
    WRITE_SYSREG_64(ttbr1_el1, (uint64_t)pagemap.top_level[1]);
    // Ok for now it should *just* work
}

void map_page(pagemap_t pagemap, uint64_t virt_addr, uint64_t phys_addr, uint64_t flags, enum page_size pg_size) {
    // Calculate the indices in the various tables using the virtual address
    size_t pml5_entry = (virt_addr & ((uint64_t)0xf << 48)) >> 48;
    size_t pml4_entry = (virt_addr & ((uint64_t)0x1ff << 39)) >> 39;
    size_t pml3_entry = (virt_addr & ((uint64_t)0x1ff << 30)) >> 30;
    size_t pml2_entry = (virt_addr & ((uint64_t)0x1ff << 21)) >> 21;
    size_t pml1_entry = (virt_addr & ((uint64_t)0x1ff << 12)) >> 12;

    pt_entry_t *pml5, *pml4, *pml3, *pml2, *pml1;

    bool is_higher_half = virt_addr & ((uint64_t)1 << 63);

    uint64_t real_flags = PT_FLAG_VALID | PT_FLAG_INNER_SH | PT_FLAG_ACCESS | PT_FLAG_WB;
    if (!(flags & VMM_FLAG_WRITE))
        real_flags |= PT_FLAG_READONLY;
    if (flags & VMM_FLAG_NOEXEC)
        real_flags |= PT_FLAG_XN;
    if (flags & VMM_FLAG_FB)
        real_flags |= PT_FLAG_FB;

    // Paging levels
    switch (pagemap.levels) {
        case 5:
            pml5 = pagemap.top_level[is_higher_half];
            goto level5;
        case 4:
            pml4 = pagemap.top_level[is_higher_half];
            goto level4;
        default:
            __builtin_unreachable();
    }

level5:
    pml4 = get_next_level(pagemap, pml5, virt_addr, pg_size, 4, pml5_entry);
level4:
    pml3 = get_next_level(pagemap, pml4, virt_addr, pg_size, 3, pml4_entry);

    if (pg_size == Size1GiB) {
        pml3[pml3_entry] = (pt_entry_t)(phys_addr | real_flags | PT_FLAG_BLOCK);
        return;
    }

    pml2 = get_next_level(pagemap, pml3, virt_addr, pg_size, 2, pml3_entry);

    if (pg_size == Size2MiB) {
        pml2[pml2_entry] = (pt_entry_t)(phys_addr | real_flags | PT_FLAG_BLOCK);
        return;
    }

    pml1 = get_next_level(pagemap, pml2, virt_addr, pg_size, 1, pml2_entry);

    pml1[pml1_entry] = (pt_entry_t)(phys_addr | real_flags | PT_FLAG_4K_PAGE);
}

static pt_entry_t *get_next_level(pagemap_t pagemap, pt_entry_t *current_level,
                                  uint64_t virt, enum page_size desired_sz,
                                  size_t level_idx, size_t entry) {
    pt_entry_t *ret;

    if (PT_IS_TABLE(current_level[entry])) {
        ret = (pt_entry_t *)(size_t)pte_addr(current_level[entry]);
    } else {
        if (PT_IS_LARGE(current_level[entry])) {
            // We are replacing an existing large page with a smaller page.
            // Split the previous mapping into mappings of the newly requested size
            // before performing the requested map operation.


            if ((level_idx >= VMM_MAX_LEVEL) || (level_idx == 0))
                panic(false, "Unexpected level in get_next_level");
            if (desired_sz >= VMM_MAX_LEVEL)
                panic(false, "Unexpected page size in get_next_level");

            uint64_t old_page_size = page_sizes[level_idx];
            uint64_t new_page_size = page_sizes[desired_sz];

            // Save all the information from the old entry at this level
            uint64_t old_flags = PT_TO_VMM_FLAGS(current_level[entry]);
            uint64_t old_phys = pte_addr(current_level[entry]);
            uint64_t old_virt = virt & ~(old_page_size - 1);

            if (old_phys & (old_page_size - 1))
                panic(false, "Unexpected page table entry address in get_next_level");

            // Allocate a table for the next level
            ret = kmalloc(PT_SIZE);
            current_level[entry] = pte_new((size_t)ret, PT_TABLE_FLAGS);

            // Recreate the old mapping with smaller pages
            for (uint64_t i = 0; i < old_page_size; i += new_page_size) {
                map_page(pagemap, old_virt + i, old_phys + i, old_flags, desired_sz);
            }
        } else {
            // Allocate a table for the next level
            ret = kmalloc(PT_SIZE);
            current_level[entry] = pte_new((size_t)ret, PT_TABLE_FLAGS);
        }
    }

    return ret;
}