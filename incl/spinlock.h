#pragma once
#include <stdatomic.h>

void acquire_mutex(atomic_flag* mutex);
void release_mutex(atomic_flag* mutex);

typedef atomic_flag imutex;
