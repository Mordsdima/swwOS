// VMM for MMIO/user-land memory
// as kernel-land memory (not mmio) is *already* done by bootloader

// riscv part is took from limine but ported to be used in swwOS
// so i guess this file should be under bsd2 license
// so this file is licensed under bsd 2 clause license

#include <fb.h>
#include <liballoc.h>
#include <vmm.h>

#define PT_SIZE ((uint64_t)0x1000)

typedef uint64_t pt_entry_t;

static uint64_t page_sizes[5] = {
  0x1000,
  0x200000,
  0x40000000,
  0x8000000000,
  0x1000000000000,
};

#define PT_FLAG_VALID ((uint64_t)1 << 0)
#define PT_FLAG_READ ((uint64_t)1 << 1)
#define PT_FLAG_WRITE ((uint64_t)1 << 2)
#define PT_FLAG_EXEC ((uint64_t)1 << 3)
#define PT_FLAG_USER ((uint64_t)1 << 4)
#define PT_FLAG_ACCESSED ((uint64_t)1 << 6)
#define PT_FLAG_DIRTY ((uint64_t)1 << 7)
#define PT_FLAG_PBMT_NC ((uint64_t)1 << 62)
#define PT_PADDR_MASK ((uint64_t)0x003ffffffffffc00)

#define PT_FLAG_RWX (PT_FLAG_READ | PT_FLAG_WRITE | PT_FLAG_EXEC)

#define PT_TABLE_FLAGS PT_FLAG_VALID
#define PT_IS_TABLE(x) (((x) & (PT_FLAG_VALID | PT_FLAG_RWX)) == PT_FLAG_VALID)
#define PT_IS_LARGE(x) (((x) & (PT_FLAG_VALID | PT_FLAG_RWX)) > PT_FLAG_VALID)
#define PT_TO_VMM_FLAGS(x) (pt_to_vmm_flags_internal(x))

#define pte_new(addr, flags) (((pt_entry_t)(addr) >> 2) | (flags))
#define pte_addr(pte) (((pte) & PT_PADDR_MASK) << 2)

static pt_entry_t* get_next_level(pagemap_t pagemap, pt_entry_t* current_level,
    uint64_t virt, enum page_size desired_sz,
    size_t level_idx, size_t entry);

static uint64_t pt_to_vmm_flags_internal(pt_entry_t entry)
{
  uint64_t flags = 0;

  if (entry & PT_FLAG_WRITE)
    flags |= VMM_FLAG_WRITE;
  if (!(entry & PT_FLAG_EXEC))
    flags |= VMM_FLAG_NOEXEC;
  if (entry & PT_FLAG_PBMT_NC)
    flags |= VMM_FLAG_FB;

  return flags;
}

void map_page(pagemap_t pagemap, uint64_t virt_addr, uint64_t phys_addr, uint64_t flags, enum page_size page_size)
{
  // Truncate the requested page size to the maximum supported.
  if (page_size > pagemap.max_page_size)
    page_size = pagemap.max_page_size;

  // Convert VMM_FLAG_* into PT_FLAG_*.
  // Set the ACCESSED and DIRTY flags to avoid faults.
  pt_entry_t ptflags = PT_FLAG_VALID | PT_FLAG_READ | PT_FLAG_ACCESSED | PT_FLAG_DIRTY;
  if (flags & VMM_FLAG_WRITE)
    ptflags |= PT_FLAG_WRITE;
  if (!(flags & VMM_FLAG_NOEXEC))
    ptflags |= PT_FLAG_EXEC;

  // Start at the highest level.
  // The values of `enum page_size` map to the level index at which that size is mapped.
  int level = pagemap.max_page_size;
  pt_entry_t* table = pagemap.top_level;
  for (;;) {
    int index = (virt_addr >> (12 + 9 * level)) & 0x1ff;

    // Stop when we reach the level for the requested page size.
    if (level == (int)page_size) {
      table[index] = pte_new(phys_addr, ptflags);
      break;
    }

    table = get_next_level(pagemap, table, virt_addr, page_size, level, index);
    level--;
  }
}

static pt_entry_t* get_next_level(pagemap_t pagemap, pt_entry_t* current_level,
    uint64_t virt, enum page_size desired_sz,
    size_t level_idx, size_t entry)
{
  pt_entry_t* ret;

  if (PT_IS_TABLE(current_level[entry])) {
    ret = (pt_entry_t*)(size_t)pte_addr(current_level[entry]);
  } else {
    if (PT_IS_LARGE(current_level[entry])) {
      // We are replacing an existing large page with a smaller page.
      // Split the previous mapping into mappings of the newly requested size
      // before performing the requested map operation.

      if ((level_idx >= VMM_MAX_LEVEL) || (level_idx == 0)) {
        raw_print("unexp level!\n");
        for (;;)
          ;
      }
      if (desired_sz >= VMM_MAX_LEVEL) {
        raw_print("unexp page sz!\n");
        for (;;)
          ;
      }

      uint64_t old_page_size = page_sizes[level_idx];
      uint64_t new_page_size = page_sizes[desired_sz];

      // Save all the information from the old entry at this level
      uint64_t old_flags = PT_TO_VMM_FLAGS(current_level[entry]);
      uint64_t old_phys = pte_addr(current_level[entry]);
      uint64_t old_virt = virt & ~(old_page_size - 1);

      if (old_phys & (old_page_size - 1)) {
        raw_print("unexp pte addr!\n");
        for (;;)
          ;
      }
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

void map_pages(pagemap_t pagemap, uint64_t virt, uint64_t phys, uint64_t flags, uint64_t count)
{
  if (virt % 0x1000 != 0 || phys % 0x1000 != 0 || count % 0x1000 != 0) {
    panic(true, "vmm: Misaligned call to map_pages()");
  }

  for (uint64_t i = 0; i < count;) {
    if (((phys + i) & (0x40000000 - 1)) == 0 && ((virt + i) & (0x40000000 - 1)) == 0 && count - i >= 0x40000000) {
      map_page(pagemap, virt + i, phys + i, flags, Size1GiB);
      i += 0x40000000;
      continue;
    }
    if (((phys + i) & (0x200000 - 1)) == 0 && ((virt + i) & (0x200000 - 1)) == 0 && count - i >= 0x200000) {
      map_page(pagemap, virt + i, phys + i, flags, Size2MiB);
      i += 0x200000;
      continue;
    }
    map_page(pagemap, virt + i, phys + i, flags, Size4KiB);
    i += 0x1000;
  }
}
