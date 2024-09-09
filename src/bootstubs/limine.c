// Limine boot stub

#include <liminereqs.h>
#include <limine.h>
#include <stdbool.h>
#include <panic.h>
#include <bootstub.h>

extern void kmain();

fb_info_t limine_get_fb(uint8_t fb_num) {
    struct limine_framebuffer *lfb = framebuffer_request.response->framebuffers[fb_num];
    fb_info_t fb = {
        .address = lfb->address,
        .width = lfb->width,
        .height = lfb->height,
        .pitch = lfb->pitch,
        .bpp = lfb->bpp
    };

    return fb;
}

uint64_t limine_get_hhdm() {
    return hhdm_request.response->offset;
}

mmap_entries_t limine_get_mmap() {
    mmap_entries_t me = {
        .entry_count = memmap_request.response->entry_count,
        .entries = (mmap_entry_t**)memmap_request.response->entries
    };

    return me;
}

module_t limine_get_module() {
    module_t mod = {
        .addr = NULL
    };
    if(_modules.response->module_count == 0) {
        return mod;
    }

    mod.addr = _modules.response->modules[0]->address;
    mod.size = _modules.response->modules[0]->size;

    /*module_t mod = {
        .addr = _modules.internal_modules[0]
    }*/

    return mod;
}

char* limine_get_cmdline() {
    return kernel_file.response->kernel_file->cmdline;
}

void limine_start() {
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        early_panic();
    }

    bootstub_t bs = {
        .get_fb = limine_get_fb,
        .get_mmap = limine_get_mmap,
        .get_hhdm_off = limine_get_hhdm,
        .get_module = limine_get_module,
        .get_cmdline = limine_get_cmdline
    };

    kmain(&bs); // Lets call kernel!
}