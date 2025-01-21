#pragma once

#include <stdint.h>

// we dont have classes or similar in c
// so this is struct yes
struct interrupt_controller {
    void* data;
    uint32_t (*claim)(struct interrupt_controller*);
    void (*complete)(struct interrupt_controller*, uint32_t);
    void (*threshold)(struct interrupt_controller*, uint8_t);
    void (*enable)(struct interrupt_controller*, uint32_t);
    void (*prio)(struct interrupt_controller*, uint32_t, uint8_t);
};
