#include <stdint.h>

#pragma once

typedef struct {
    uint64_t size;
    void* addr;
} module_t;

typedef struct {
    uint64_t base;
    uint64_t length;
    uint64_t type;
} mmap_entry_t;

typedef struct {
    uint64_t entry_count;
    mmap_entry_t **entries;
} mmap_entries_t;

typedef struct {
    void *address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint16_t bpp; // Bits per pixel
} fb_info_t;

typedef struct {
    fb_info_t      (*get_fb)   (uint8_t);
    uint64_t       (*get_hhdm_off)(void); // on linux payload just return 0
    mmap_entries_t (*get_mmap)    (void);
    module_t       (*get_module)  (void);
    char*          (*get_cmdline) (void);
} bootstub_t;

