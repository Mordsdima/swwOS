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

extern uint64_t _exception_vector;

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

void kfault() {
    asm volatile("wfe");
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

    tprintf("Booted via %s (version: %s)\n", bootloader_info.response->name, bootloader_info.response->version);

    log_info("Initializating basic stuff... (such as exception vector)");

    set_vbar_el1(&_exception_vector); // Init interrupts
    init_pmm(); // Init PMM
    vfs_init(); // Init VFS
    
    

    uint64_t* a = (uint64_t*)kmalloc(0x1000);
    tprintf("%x", a);
    *a = 1;

    //uint64_t* a = (uint64_t*)kmalloc_pages(1);

    //tprintf("%x", a);

    log_info("uwu");

    //log_info("kernel ");

    for (;;);
}

