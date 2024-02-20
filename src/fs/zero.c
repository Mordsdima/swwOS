// this is example dont use it.. please..

#include <vfs.h>
#include <liballoc.h>
#include <string.h>
#include <terminal.h>

long zero_read(vfs_node_t* node, char* path, size_t off, size_t len, uint8_t* buf) {
    tprintf("zero read: \noff: %x\nlen: %x\nbuf: %x\npath: %s\ndrive_id: %x", off, len, buf, path, node->id);
    return 0;
}

long zero_write(vfs_node_t* node, char* path, size_t off, size_t len, uint8_t* buf) {
    tprintf("zero write: \noff: %x\nlen: %x\nbuf: %x\npath: %s\ndrive_id: %x", off, len, buf, path, node->id);
    return len;
}

void initialize_zerofs() {
    vfs_node_t* zero_drive = kmalloc(sizeof(vfs_node_t));
    strcpy(zero_drive->name, "test");
    zero_drive->device = NULL;
    zero_drive->read = zero_read;
    zero_drive->write = zero_write;
    zero_drive->ioctl = NULL;
    zero_drive->open = NULL;
    zero_drive->close = NULL;
    vfs_mount(zero_drive);
}