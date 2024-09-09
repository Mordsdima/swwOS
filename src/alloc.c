#include <locker.h>
#include <pmm.h>

locker_t locker;
extern uint64_t hhdm_off;
#define MAP_TO_HHDM(addr) (hhdm_off + (uint64_t)(addr))

int liballoc_lock() {
    lock(locker);
    return 0;
}

int liballoc_unlock() {
    unlock(locker);
    return 0;
}

void* liballoc_alloc(int pages) {
    return (void*)MAP_TO_HHDM(kmalloc_pages(pages));
}

int liballoc_free(void* ptr, int pages) {
    kfree_pages((uint64_t)ptr, pages);
    return 0;
}