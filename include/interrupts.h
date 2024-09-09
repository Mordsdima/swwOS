#pragma once

#include <utils.h>
#include <stdint.h>

#ifdef __aarch64__
extern uint64_t _exception_vector;
#elif defined(__riscv)
extern void _trap_handler();
#endif

inline static void int_init() {
    #ifdef __aarch64__
    set_vbar_el1(&_exception_vector);
    #elif defined(__riscv)
    set_stvec(&_trap_handler);
    #endif
}