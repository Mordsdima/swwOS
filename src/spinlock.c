#include <stdatomic.h>

void acquire_mutex(atomic_flag* mutex)
{
  while (atomic_flag_test_and_set(mutex)) { } // do nothing
}

void release_mutex(atomic_flag* mutex)
{
  atomic_flag_clear(mutex);
}
