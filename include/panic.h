#include <terminal.h>
#include <stdarg.h>

#pragma once

static inline void early_panic() {
    // 0xDEADBEEF
    asm volatile("ldr x0, =0xdeadbeef");
    asm("wfi");
    asm("wfe");
    for(;;)
        asm("nop");
}

static inline void panic(const char* restrict format, ...) {
    clear();
    va_list parameters;
	va_start(parameters, format);

    tprintf("A critical error has occurred (panic):\n");
	tvsprintf(format, parameters);

	va_end(parameters);

    early_panic();
}