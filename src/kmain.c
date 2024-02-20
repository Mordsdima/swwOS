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

extern uint64_t _exception_vector;
extern void initialize_zerofs();

LIMINE_BASE_REVISION(1)

static void kmain(void);

struct limine_entry_point_request entry_point_request = {
    .id = LIMINE_ENTRY_POINT_REQUEST,
    .revision = 0, .response = NULL,

    .entry = kmain
};

struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0, .response = NULL
};

struct limine_bootloader_info_request bootloader_info = {
    .id = LIMINE_BOOTLOADER_INFO_REQUEST,
    .revision = 0, .response = NULL
};

struct limine_stack_size_request _stack = {
    .id = LIMINE_STACK_SIZE_REQUEST,
    .revision = 0, .response = NULL,

    .stack_size = 128*1024 // 128 kb
};

struct limine_module_request _modules = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 2, .response = NULL
};

void kfault() {
    asm("wfe");
    for(;;);
}

static void kmain() {
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        early_panic();
    }

    if (framebuffer_request.response == NULL
     || framebuffer_request.response->framebuffer_count < 1) {
        early_panic();
    }

    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];

    terminal_init(fb);

    tprintf("Welcome to swwk! Built at %s %s\n", __DATE__, __TIME__);
    tprintf("Booted via %s (version: %s)\n", bootloader_info.response->name, bootloader_info.response->version);

    log_info("Initializating basic stuff... (such as exception vector)");

    set_vbar_el1(&_exception_vector); // Init interrupts
    init_pmm(); // Init PMM
    vfs_init(); // Init VFS
    
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

    for (;;);
}

