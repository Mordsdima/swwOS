#include <locker.h>
#include <pmm.h>

locker_t locker;
extern struct limine_hhdm_request hhdm_request;
#define MAP_TO_HHDM(addr) (hhdm_request.response->offset + (uint64_t)(addr))

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