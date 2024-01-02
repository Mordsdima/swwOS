#include <limine.h>
#include <stdbool.h>
#include <stddef.h>
#include <panic.h>
#include <terminal.h>
#include <log.h>
#include <acpi.h>
#include <utils.h>
#include <liminereqs.h>

extern uint64_t _exception_vector;


void process_memory(struct limine_memmap_entry* e) {
    switch(e->type) {
        case LIMINE_MEMMAP_FRAMEBUFFER:
            log_info("Found a memory block - Framebuffer");
            break;
        case LIMINE_MEMMAP_USABLE:
            log_info("Found a memory block - Usable");
            break;
        case LIMINE_MEMMAP_RESERVED:
            log_info("Found a memory block - Reserved");
            break;
        case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
            log_info("Found a memory block - ACPI Reclaimable.");
            break;
    }
}

void kfault() {
    panic("Fault!");
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

    log_info("basic io init ok");

    log_info("setupping vector table (interrupts omg)");

    set_vbar_el1(&_exception_vector);

    /*struct limine_memmap_response *memmap_response = memmap_request.response;
    for (size_t i = 0; i < memmap_response->entry_count; i++) {
        struct limine_memmap_entry *e = memmap_response->entries[i];
        process_memory(e);
    }*/
    init_rsdp(_rsdp.response->address);

    log_info("uwu");

    //log_info("kernel ");

    for (;;);
}

