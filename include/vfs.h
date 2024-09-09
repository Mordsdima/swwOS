#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#pragma once

#define MAX_ID 65535

typedef struct {
    bool is_allocated[MAX_ID + 1];
} id_pool_t;

typedef struct vfs_node {
    char name[512];

    uint16_t id;

    void* device;

    long (*read)(struct vfs_node*, char*, size_t, size_t, uint8_t*);
    long (*write)(struct vfs_node*, char*, size_t, size_t, uint8_t*);
    long (*getsize)(struct vfs_node*, char*);
    long (*ioctl)(struct vfs_node*, char*, uint64_t, void*);
    long (*open)(struct vfs_node*, char*, int);
    long (*close)(struct vfs_node*, char*);

    struct vfs_node *prev;
    struct vfs_node *next;

} vfs_node_t;

int vfs_init();
long vfs_read(char* path, size_t offset, size_t len, uint8_t* buf);
long vfs_write(char* path, size_t offset, size_t len, uint8_t* buf);
long vfs_getsize(char* path);
long vfs_open(char* path);
long vfs_close(char* path);
int vfs_mount(vfs_node_t* node);
