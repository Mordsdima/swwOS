#include <locker.h>

void lock(locker_t locker) {
    if(!locker.locked) 
        locker.locked = true;
}

void unlock(locker_t locker) {
    if(locker.locked) 
        locker.locked = false;
}

bool is_locked(locker_t locker) {
    return locker.locked;
}

bool is_unlocked(locker_t locker) {
    return !locker.locked;
}