#include <stdint.h>
#include <limine.h>
#include <stddef.h>

#ifndef _LIMINE_REQUESTS_H_
#define _LIMINE_REQUESTS_H_

struct limine_rsdp_request _rsdp = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0, .response = NULL
};

#endif