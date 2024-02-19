#include <pmm.h>
#include <vfs.h>
#include <errno.h>



int vfs_init() {
    if(!pmm_is_initializated()) {
        return -EIO;
    }

    // todo

    return EOK;
}