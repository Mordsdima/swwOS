#pragma once

#ifdef __aarch64__
#define WRITE_SYSREG(sysreg, val, type) \
    asm volatile ("msr        "#sysreg", %0\n" : : "r"((type)(val)))
#define READ_SYSREG(sysreg, val, type) \
    asm volatile ("mrs        %0, "#sysreg"\n" : "=r"((type)(val)))
#elif defined(__riscv)
#define WRITE_SYSREG(sysreg, val, type) \
    asm volatile ("csrw        "#sysreg", %0\n" : : "r"((type)(val)))
#define READ_SYSREG(sysreg, val, type) \
    asm volatile ("csrr        %0, "#sysreg"\n" : "=r"((type)(val)))
#endif

#define READ_SYSREG_32(sysreg, val)   READ_SYSREG(sysreg, val, uint32_t)
#define WRITE_SYSREG_32(sysreg, val)  WRITE_SYSREG(sysreg, val, uint32_t)

#define READ_SYSREG_64(sysreg, val)   READ_SYSREG(sysreg, val, uint64_t)
#define WRITE_SYSREG_64(sysreg, val)  WRITE_SYSREG(sysreg, val, uint64_t)

#ifdef __aarch64__
inline static void set_vbar_el1(void * address) {
    WRITE_SYSREG_64(vbar_el1, (uint64_t)address);
}
#elif defined(__riscv)
inline static void set_stvec(void* address) {
    WRITE_SYSREG_64(stvec, (uint64_t)address);
}
#endif

static inline long strlen(const char* string) {
    long i;

    for (i = 0; string[i] != '\0'; ++i);

    return i;
}