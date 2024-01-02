#include <stdint.h>
#include <limine.h>
#include <stddef.h>

#ifndef _LIMINE_REQUESTS_H_
#define _LIMINE_REQUESTS_H_

static void kmain(void);

LIMINE_BASE_REVISION(1)

struct limine_entry_point_request entry_point_request = {
    .id = LIMINE_ENTRY_POINT_REQUEST,
    .revision = 0, .response = NULL,

    .entry = kmain
};

struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0, .response = NULL
};

struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
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

struct limine_rsdp_request _rsdp = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0, .response = NULL
};

#endif