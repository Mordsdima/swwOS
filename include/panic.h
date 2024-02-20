#include <terminal.h>
#include <stdarg.h>

#ifndef _PANIC_H_
#define _PANIC_H_

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

    tprintf("swwk moment! (in other words panic). Here is reason:\n");
	tvsprintf(format, parameters);

	va_end(parameters);

    early_panic();
}
#endif