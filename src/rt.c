#include <stdint.h>

uint64_t __clzdi2(uint64_t x) {
    if (x == 0) return 64;
    uint64_t n = 0;
    if (x <= 0x00000000FFFFFFFF) { n += 32; x <<= 32; }
    if (x <= 0x0000FFFFFFFFFFFF) { n += 16; x <<= 16; }
    if (x <= 0x00FFFFFFFFFFFFFF) { n += 8; x <<= 8; }
    if (x <= 0x0FFFFFFFFFFFFFFF) { n += 4; x <<= 4; }
    if (x <= 0x3FFFFFFFFFFFFFFF) { n += 2; x <<= 2; }
    if (x <= 0x7FFFFFFFFFFFFFFF) { n += 1; }
    return n;
}

uint64_t __ffsdi2(uint64_t x) {
    if (x == 0) return 0; // Если x равно 0, то возвращаем 0, так как нет установленных битов
    return __builtin_ffsll(x); // Используем встроенную функцию компилятора для этого
}

uint64_t __popcountdi2(uint64_t x) {
    uint64_t count = 0;
    while (x) {
        count += x & 1;  
        x >>= 1;        
    }
    return count;
}

#ifdef __aarch64__
#include <stdatomic.h>

int __aarch64_cas4_acq_rel(volatile int *ptr, int oldval, int newval) {
    return __sync_val_compare_and_swap(ptr, oldval, newval);
}

int __aarch64_cas4_sync(volatile int *ptr, int oldval, int newval) {
    return __sync_val_compare_and_swap(ptr, oldval, newval);
}

#endif
