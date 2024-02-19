#include <pmm.h>
#include <vfs.h>
#include <errno.h>
#include <locker.h>

locker_t vfs_initializated;
vfs_node_t* current_node;

long vfs_total_files() {
    do {
        current_node = current_node->prev;
    } while (current_node->prev != NULL);
    long i = 0;
    do {
        current_node = current_node->next; i++;
    } while (current_node->next != NULL);

    return i;
}

int vfs_init() {
    if(!pmm_is_initializated()) {
        return -EIO;
    }
    if(is_locked(vfs_initializated)) {
        return -EPERM;
    }

    lock(vfs_initializated);

    return EOK;
}

int vfs_mount(vfs_node_t* node) {
    do {
        current_node = current_node->next;
    } while (current_node->next != NULL);
    current_node->next = node;

    return EOK;
}

