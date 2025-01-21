#include <vmm.h>

extern uint64_t hhdm_off;

#define MAP_TO_HHDM(addr) (hhdm_off + (uint64_t)(addr))
