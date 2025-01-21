#include <fb.h>
#include <limine.h>
#include <pmm.h>
#include <spinlock.h>
#include <stdbool.h>
#include <stddef.h>

#define MAP_TO_HHDM(addr) (hhdm_off + (uint64_t)(addr))

__attribute__((used, section(".limine_requests"))) static volatile struct limine_memmap_request memmap_request = {
  .id = LIMINE_MEMMAP_REQUEST,
  .revision = 0
};

__attribute__((used, section(".limine_requests"))) static volatile struct limine_hhdm_request hhdm_req = {
  .id = LIMINE_HHDM_REQUEST,
  .revision = 0
};

imutex pmm_mutex;
bool pmm_initializated;
size_t memory_nodes;
memory_node_t *memory_head, *memory_tail;
uint64_t hhdm_off = 0;

void init_pmm()
{
  if (pmm_initializated == true)
    return;
  acquire_mutex(&pmm_mutex);

  struct limine_memmap_entry** entries = memmap_request.response->entries;
  size_t total_entries = memmap_request.response->entry_count;
  hhdm_off = hhdm_req.response->offset;

  for (size_t i = 0; i < total_entries; i++) {
    struct limine_memmap_entry* entry = entries[i];

    if (entry->type != LIMINE_MEMMAP_USABLE)
      continue;

    uint64_t base = entry->base;

    if (base < 0x1000)
      base = 0x1000;

    if (base & 0xfff)
      base = (base + 0xfff) & 0xfff;

    size_t size = entry->length;

    memory_nodes++;

    memory_node_t* new_node = (memory_node_t*)MAP_TO_HHDM(base);
    memory_node_t* new_node_phys = (memory_node_t*)base;

    if (memory_tail)
      ((memory_node_t*)MAP_TO_HHDM(memory_tail))->next = new_node_phys;
    if (!memory_head)
      memory_head = new_node_phys;

    new_node->prev = memory_tail;
    new_node->next = NULL;
    new_node->pages = size / 0x1000;

    memory_tail = new_node_phys;
  }
  release_mutex(&pmm_mutex);
}

uint64_t* kmalloc_pages(size_t pages)
{
  if (!memory_nodes) {
    raw_print("PMM: Not initialized.\n");
    return NULL;
  }

  acquire_mutex(&pmm_mutex);

  memory_node_t* node = (memory_node_t*)MAP_TO_HHDM((uint64_t*)memory_head);
  memory_node_t* node_phys = memory_head;

  uint64_t ret = (uint64_t)memory_head;

  while (node->pages < pages) {
    node = node->next;
    if (!node) {
      raw_print("PMM: No left available nodes!\n");
      return NULL;
    }

    node_phys = node;
    node = (memory_node_t*)MAP_TO_HHDM((uint64_t*)node);
  }

  node->pages -= pages;

  if (!node->pages) {
    memory_node_t *next = node->next, *prev = node->prev;

    if (next)
      ((memory_node_t*)MAP_TO_HHDM((uint64_t*)next))->prev = prev;

    if (prev)
      ((memory_node_t*)MAP_TO_HHDM((uint64_t*)prev))->next = next;

    if (memory_head == node_phys)
      memory_head = next;

    if (memory_tail == node_phys)
      memory_tail = prev;

    node->next = NULL;
    node->prev = NULL;
  }

  ret = ret + node->pages * 4096;

  release_mutex(&pmm_mutex);
  pmm_initializated = true;
  return (uint64_t*)ret;
}

void kfree_pages(uint64_t* addr, size_t pages)
{
  acquire_mutex(&pmm_mutex);

  memory_node_t* node = (memory_node_t*)MAP_TO_HHDM(addr);
  memory_node_t* node_phys = (memory_node_t*)addr;
  if (memory_tail)
    ((memory_node_t*)MAP_TO_HHDM((uint64_t*)memory_tail))->next = node_phys;

  if (!memory_head)
    memory_head = node_phys;

  node->next = NULL;
  node->prev = memory_tail;
  memory_tail = node_phys;
  node->pages = pages;
  memory_nodes++;
  release_mutex(&pmm_mutex);
}

imutex lalloc_mutex;

int liballoc_lock()
{
  release_mutex(&lalloc_mutex);
  return 0;
}

int liballoc_unlock()
{
  acquire_mutex(&lalloc_mutex);
  return 0;
}

void* liballoc_alloc(int pages)
{
  return (void*)MAP_TO_HHDM(kmalloc_pages(pages));
}

int liballoc_free(void* ptr, int pages)
{
  kfree_pages(ptr, pages);
  return 0;
}
