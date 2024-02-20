#include <pmm.h>
#include <liballoc.h>
#include <vfs.h>
#include <errno.h>
#include <locker.h>
#include <string.h>
#include <terminal.h>
#include <log.h>

locker_t vfs_initializated;
vfs_node_t* first_node;
uint16_t total_mounted = 0;

uint16_t divide_path(char* path) {
    char *split_point = strchr(path, ':');
    if(split_point != NULL) {
        uint16_t x = (uint16_t)strtol(path, NULL, 10);
        path = split_point+1;
        return x;
    } else {
        return 0;
    }
}
int vfs_init() {
    if(!pmm_is_initializated()) {
        return -EIO;
    }
    if(is_locked(vfs_initializated)) {
        return -EPERM;
    }

    lock(vfs_initializated);

    first_node = NULL;

    return EOK;
}

int vfs_mount(vfs_node_t* node) {
    vfs_node_t* current_node = NULL;

    if(!first_node) {
        first_node = node;
        return EOK;
    }

    current_node = first_node->next;

    while(current_node->next)
        current_node = current_node->next;
    current_node->next = node;

    return EOK;
}

long vfs_read(char* path, size_t offset, size_t len, uint8_t* buf) {
    if(!first_node) 
        return -ENXIO;
    vfs_node_t* current_node = first_node;
    tprintf("%s", path);
    uint16_t id = divide_path(path);

    while(current_node->id != id && current_node->next && current_node)
        current_node = current_node->next;

    if(current_node == NULL) 
        return -ENXIO;
    if(!current_node->write)
        return -EINVAL; // what fucking device dont have a read/write?

    return current_node->read(current_node, path, offset, len, buf);
}

long vfs_write(char* path, size_t offset, size_t len, uint8_t* buf) {
    if(!first_node) 
        return -ENXIO;
    vfs_node_t* current_node = first_node;
    tprintf("%s", path);
    uint16_t id = divide_path(path);

    while(current_node->id != id && current_node->next && current_node)
        current_node = current_node->next;

    if(current_node == NULL) 
        return -ENXIO;
    if(!current_node->write)
        return -EINVAL; // what fucking device dont have a read/write?

    return current_node->write(current_node, path, offset, len, buf);
}

long vfs_open(char* path) {
    tprintf("tried to access %s while open is not implemented\n", path);
    return EOK; // boilerplate xdxd
}

long vfs_close(char* path) {
    tprintf("tried to access %s while close is not implemented\n", path);
    return EOK; // same as vfs_open
}

