#include <stdbool.h>

#pragma once
typedef struct locker {
    bool locked;
} locker_t;

void lock(locker_t locker);
void unlock(locker_t locker);
bool is_locked(locker_t locker);
bool is_unlocked(locker_t locker);