#include <stdbool.h>

#ifndef _LOCKER_H
#define _LOCKER_H

typedef struct locker {
    bool locked;
} locker_t;

void lock(locker_t locker);
void unlock(locker_t locker);
bool is_locked(locker_t locker);
bool is_unlocked(locker_t locker);

#endif