#include <limine.h>
#include <stdbool.h>
#include <stddef.h>
#include <panic.h>
#include <terminal.h>
#include <log.h>
#include <utils.h>
#include <limine.h>
#include <pmm.h>
#include <vfs.h>
#include <liballoc.h>
#include <fs/tar.h>
#include <bootstub.h>
#include <interrupts.h>

extern uint64_t _exception_vector;
extern void initialize_zerofs();

void kfault() {
    for(;;);
}

void kmain(bootstub_t* bs) {
    fb_info_t fb = bs->get_fb(0);

    terminal_init(&fb);

    tprintf("Welcome to swwk! Built at %s %s\n", __DATE__, __TIME__);

    log_info("Initializating basic stuff... (such as exception vector)");
    
    int_init();//set_vbar_el1(&_exception_vector); // Init interrupts
    
    init_pmm(bs); // Init PMM
    vfs_init(); // Init VFS
    

    module_t module = bs->get_module();

    if(module.addr == NULL) {
        panic("no module (initrd) found so nothing to do");
    }

    int initrd_mount_result = vfs_mount(init_tar(module.addr));
    if(initrd_mount_result < 0) {
        panic("Oh no! Failed to mount filesystem: %x", initrd_mount_result);
    }
    long size = vfs_getsize(bs->get_cmdline());
    uint8_t* buf = kmalloc(size);//

    vfs_read(bs->get_cmdline(), 0, size, buf);
    
    print((char*)buf, size);
    //tprintf("%s\n", *buf);

    log_info("Thats end!");

    /*
    tprintf("Booted via %s (version: %s)\n", bootloader_info.response->name, bootloader_info.response->version);

    
    
    if(!_modules.response) {
        panic("initrd not found. nothing more to do");
    }

    struct limine_file *initrd = _modules.response->modules[0];

    int initrd_mount_result = vfs_mount(init_tar(initrd->address));
    if(initrd_mount_result < 0) {
        panic("Oh no! Failed to mount filesystem: %x", initrd_mount_result);
    }
    long size = vfs_getsize("0:/hi");
    uint8_t* buf = kmalloc(size);//

    vfs_read("0:/hi", 0, size, buf);
    
    print((char*)buf, size);
    tprintf("%s\n", *buf);

    log_info("uwu");
*/
    for (;;);
}

