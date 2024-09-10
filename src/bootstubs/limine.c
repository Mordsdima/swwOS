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
        .bpp = lfb->bpp,
        .red_mask_size = lfb->red_mask_size,
        .red_mask_shift = lfb->red_mask_shift,
        .green_mask_size = lfb->green_mask_size,
        .green_mask_shift = lfb->green_mask_shift,
        .blue_mask_size = lfb->blue_mask_size,
        .blue_mask_shift = lfb->blue_mask_shift
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
    return kernel_file_req.response->kernel_file->cmdline;
}

void* limine_get_dtb() {
    if(dtb_req.response == NULL) {
        return NULL;
    }
    return dtb_req.response->dtb_ptr;
}

void* limine_get_rsdp() {
    return _rsdp.response->address;
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
        .get_cmdline = limine_get_cmdline,
        .get_dtb = limine_get_dtb,
        .get_rsdp = limine_get_rsdp
    };

    kmain(&bs); // Lets call kernel!
}