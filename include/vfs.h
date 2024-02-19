#include <stdint.h>
#include <stddef.h>

#ifndef _VFS_H
#define _VFS_H

int vfs_init();

typedef struct vfs_node {
    char name[512];

    void* device;

    long (*read)(struct vfs_node*, size_t, size_t, uint8_t*);
    long (*write)(struct vfs_node*, size_t, size_t, uint8_t*);
    long (*open)(struct vfs_node*, int);
    long (*close)(struct vfs_node*);

    struct vfs_node *prev;
    struct vfs_node *next;

} vfs_node_t;

#endif