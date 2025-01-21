#include <spinlock.h>
#include <stdatomic.h>

void acquire_mutex(imutex* mutex)
{
  while (atomic_flag_test_and_set((atomic_flag*)mutex)) { } // do nothing
}

void release_mutex(imutex* mutex)
{
  atomic_flag_clear((atomic_flag*)mutex);
}
