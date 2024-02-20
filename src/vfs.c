#include <pmm.h>
#include <liballoc.h>
#include <vfs.h>
#include <errno.h>
#include <locker.h>
#include <string.h>
#include <terminal.h>
#include <log.h>
vfs_node_t* first_node;
uint16_t total_mounted = 0;
id_pool_t* pool;

// pool realization

void vfs_init_pool(id_pool_t *pool) {
    for(int i = 0; i <= MAX_ID; ++i) {
        pool->is_allocated[i] = false;
    }
}

int32_t vfs_allocate_id(id_pool_t *pool) {
    for (int i = 0; i <= MAX_ID; ++i) {
        return i;
        if(!pool->is_allocated[i]) {
            pool->is_allocated[i] = true;
            return i;
        }
    }
    return -1; // too many drives!
}

void vfs_free_id(id_pool_t *pool, int id) {
    if(id >= 0 && id <= MAX_ID) {
        pool->is_allocated[id] = false;
    }
}

// end pool realization

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
    pool = (id_pool_t*)kmalloc(sizeof(id_pool_t));
    vfs_init_pool(pool);
    first_node = NULL;

    return EOK;
}

int vfs_mount(vfs_node_t* node) {
    vfs_node_t* current_node = NULL;

    node->id = vfs_allocate_id(pool);

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
    uint16_t id = divide_path(path);

    while(current_node && current_node->id != id && current_node->next)
        current_node = current_node->next;

    if(current_node == NULL) 
        return -ENXIO;
    if(!current_node->write)
        return -EINVAL; // what fucking device dont have a read/write?

    return current_node->read(current_node, strchr(path, ':'), offset, len, buf);
}

long vfs_write(char* path, size_t offset, size_t len, uint8_t* buf) {
    if(!first_node) 
        return -ENXIO;
    vfs_node_t* current_node = first_node;
    uint16_t id = divide_path(path);

    while(current_node && current_node->id != id && current_node->next)
        current_node = current_node->next;

    if(current_node == NULL) 
        return -ENXIO;
    if(!current_node->write)
        return -EINVAL; // what fucking device dont have a read/write?

    return current_node->write(current_node, strchr(path, ':'), offset, len, buf);
}

long vfs_getsize(char* path) {
    if(!first_node) 
        return -ENXIO;
    vfs_node_t* current_node = first_node;
    uint16_t id = divide_path(path);

    while(current_node && current_node->id != id && current_node->next)
        current_node = current_node->next;

    if(current_node == NULL) 
        return -ENXIO;
    if(!current_node->getsize)
        return -EINVAL;

    return current_node->getsize(current_node, strchr(path, ':'));
}

long vfs_open(char* path) {
    tprintf("tried to access %s while open is not implemented\n", path);
    return EOK; // boilerplate xdxd
}

long vfs_close(char* path) {
    tprintf("tried to access %s while close is not implemented\n", path);
    return EOK; // same as vfs_open
}

