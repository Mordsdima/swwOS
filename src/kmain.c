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

#include <uacpi/uacpi.h>

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
    
    log_info("Okaaay...");
    
    if(bs->get_rsdp() == NULL && bs->get_dtb() == NULL) {
        panic("The machine does not support either ACPI or DTB");
    }

    if(bs->get_dtb() != NULL) {
        goto dtb_init;
    } else {
        goto acpi_init;
    }

dtb_init:
    goto later;
acpi_init:
    log_info("We have ACPI system!");
    tprintf("%x", (uint64_t)bs->get_rsdp());
    uacpi_init_params init_params = {
        .rsdp = (uint64_t)bs->get_rsdp(),
        .log_level = UACPI_LOG_TRACE,
        .flags = 0,
    };

    uacpi_status ret = uacpi_initialize(&init_params);
    if (uacpi_unlikely_error(ret)) {
        panic("uacpi_initialize error: %s", uacpi_status_to_string(ret));
    }
later:
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

    // Now we should call
    
    for (;;);
}

