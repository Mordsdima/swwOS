#include <fs/tar.h>
#include <vfs.h>
#include <liballoc.h>
#include <string.h>
#include <terminal.h>

int oct2bin(unsigned char *str, int size) {
    int n = 0;
    unsigned char *c = str;
    while (size-- > 0) {
        n *= 8;
        n += *c - '0';
        c++;
    }
    return n;
}

int tar_lookup(uint8_t* archive, char *filename, size_t offset, size_t length, uint8_t **out) {
    unsigned char *ptr = archive;
 
    while (!memcmp(ptr + 257, "ustar", 5)) {
        int filesize = oct2bin(ptr + 0x7c, 11);
        if (!memcmp(ptr, filename, strlen(filename) + 1)) {
            if(out)
                memcpy(out, ptr + 512 + offset, length);
                //*out = ptr + 512 + offset;
            return filesize; 
        }
        ptr += (((filesize + 511) / 512) + 1) * 512;
    }
    return 0;
}

long tar_read(vfs_node_t* node, char* path, size_t off, size_t len, uint8_t* buf) {
    tprintf("tar read: \noff: %x\nlen: %x\nbuf: %x\npath: %s\ndrive_id: %x", off, len, buf, path, node->id);
    tar_archive_t* archive = node->device;
    char* cpath = path;
    cpath++;

    return tar_lookup(archive->ptr, cpath, off, len, NULL);
}

long tar_getsize(vfs_node_t* node, char* path) {
    tar_archive_t* archive = node->device;
    char* cpath = path;
    cpath++;
    return tar_lookup(archive->ptr, cpath, 0, 0, NULL);
}

vfs_node_t* init_tar(uint8_t* ptr) {
    tar_archive_t* tar = kmalloc(sizeof(tar_archive_t));
    tar->ptr = ptr;
    vfs_node_t *drive = kmalloc(sizeof(vfs_node_t));

    strcpy(drive->name, "unknown");
    drive->device = tar;
    drive->read = tar_read;
    drive->getsize = tar_getsize;
    /*{
        .name = "unknown_tar_fs",
        .device = tar
    };*/

    return drive;
}