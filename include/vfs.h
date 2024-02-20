#include <stdint.h>
#include <stddef.h>

#ifndef _VFS_H
#define _VFS_H



typedef struct vfs_node {
    char name[512];

    uint16_t id;

    void* device;

    long (*read)(struct vfs_node*, char*, size_t, size_t, uint8_t*);
    long (*write)(struct vfs_node*, char*, size_t, size_t, uint8_t*);
    long (*ioctl)(struct vfs_node*, char*, uint64_t, void*);
    long (*open)(struct vfs_node*, char*, int);
    long (*close)(struct vfs_node*, char*);

    struct vfs_node *prev;
    struct vfs_node *next;

} vfs_node_t;

int vfs_init();
long vfs_read(char* path, size_t offset, size_t len, uint8_t* buf);
long vfs_write(char* path, size_t offset, size_t len, uint8_t* buf);
long vfs_open(char* path);
long vfs_close(char* path);
int vfs_mount(vfs_node_t* node);

#endif