#include <pmm.h>
#include <locker.h>
#include <errno.h>
#include <limine.h>
#include <stddef.h>
#include <stdbool.h>
#include <bootstub.h>
#include <log.h>

#define MAP_TO_HHDM(addr) (hhdm_off + (uint64_t)(addr))

locker_t pmm_locker;
locker_t pmm_initializated;
size_t memory_nodes;
memory_node_t *memory_head, *memory_tail;
uint64_t hhdm_off = 0;

int init_pmm(bootstub_t* bs) {
    if(is_locked(pmm_locker) || is_locked(pmm_initializated)) 
        return -EPERM;
    lock(pmm_initializated);
    lock(pmm_locker);

    hhdm_off = bs->get_hhdm_off();

    mmap_entries_t me = bs->get_mmap();

    for (size_t i = 0; i < me.entry_count; i++) {
        mmap_entry_t* entry = me.entries[i];

        if(entry->type != LIMINE_MEMMAP_USABLE) 
            continue;

        uint64_t base = entry->base;

        if(base < 0x1000)
            base = 0x1000;

        if(base & 0xfff)
            base = (base + 0xfff) & 0xfff;

        size_t size = entry->length;

        memory_nodes++;

        memory_node_t* new_node = (memory_node_t*)MAP_TO_HHDM(base);
        memory_node_t* new_node_phys = (memory_node_t*)base;

        if(memory_tail)
            ((memory_node_t*)MAP_TO_HHDM(memory_tail))->next = new_node_phys;
        if(!memory_head)
            memory_head = new_node_phys;

        new_node->prev = memory_tail;
        new_node->next = NULL;
        new_node->pages = size / 0x1000;

        memory_tail = new_node_phys;
    }
    unlock(pmm_locker);
    return EOK;
}

uint64_t kmalloc_pages(size_t pages) {
    if(!memory_nodes) 
        return -ENOMEM;

    lock(pmm_locker);

    memory_node_t* node = (memory_node_t*)MAP_TO_HHDM((uint64_t*)memory_head);
    memory_node_t* node_phys = memory_head;

    uint64_t ret = (uint64_t)memory_head;

    while(node->pages < pages) {
        node = node -> next;
        if(!node)   
            return -ENOMEM;

        node_phys = node;
        node = (memory_node_t*)MAP_TO_HHDM((uint64_t*)node);
    }

    node->pages -= pages;

    if(!node->pages) {
        memory_node_t *next = node->next, *prev = node->prev;

        if(next)
            ((memory_node_t*)MAP_TO_HHDM((uint64_t*)next))->prev = prev;

        if(prev)
            ((memory_node_t*)MAP_TO_HHDM((uint64_t*)prev))->next = next;

        if(memory_head == node_phys)
            memory_head = next;
        
        if(memory_tail == node_phys)
            memory_tail = prev;

        node->next = NULL;
        node->prev = NULL;
    }

    ret = ret + node->pages * 4096;

    unlock(pmm_locker);
    return ret;
}

void kfree_pages(uint64_t addr, size_t pages) {
    lock(pmm_locker);

    memory_node_t* node = (memory_node_t*)MAP_TO_HHDM((uint64_t*)addr);
    memory_node_t* node_phys = (memory_node_t*)addr;
    if(memory_tail)
        ((memory_node_t*)MAP_TO_HHDM((uint64_t*)memory_tail))->next = node_phys;

    if(!memory_head)
        memory_head = node_phys;

    node->next = NULL;
    node->prev = memory_tail;
    memory_tail = node_phys;
    node->pages = pages;
    memory_nodes++;
    unlock(pmm_locker);
}

bool pmm_is_initializated() {
    return is_locked(pmm_initializated);
}