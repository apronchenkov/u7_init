#ifndef U7_SPINLOCK_H_
#define U7_SPINLOCK_H_

#if __APPLE__
#include <os/lock.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef os_unfair_lock u7_spinlock;

#define U7_SPINLOCK_INIT OS_UNFAIR_LOCK_INIT

static inline void u7_spinlock_init(u7_spinlock* spinlock) {
  *spinlock = OS_UNFAIR_LOCK_INIT;
}

static inline void u7_spinlock_lock(u7_spinlock* spinlock) {
  os_unfair_lock_lock(spinlock);
}

__attribute__((__warn_unused_result__)) static inline bool u7_spinlock_trylock(
    u7_spinlock* spinlock) {
  return os_unfair_lock_trylock(spinlock);
}

static inline void u7_spinlock_unlock(u7_spinlock* spinlock) {
  os_unfair_lock_unlock(spinlock);
}

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#else  // __APPLE__

#include <stdatomic.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  atomic_flag flag;
} u7_spinlock;

#define U7_SPINLOCK_INIT \
  { .flag = ATOMIC_FLAG_INIT }

static inline void u7_spinlock_init(u7_spinlock* spinlock) {
  atomic_flag_clear_explicit(&spinlock->flag, memory_order_release);
}

int u7_spinlock_internal_yield_k(int k);

static inline void u7_spinlock_lock(u7_spinlock* spinlock) {
  int k = 0;
  while (atomic_flag_test_and_set_explicit(&spinlock->flag,
                                           memory_order_acquire)) {
    k = u7_spinlock_internal_yield_k(k);
  }
}

__attribute__((__warn_unused_result__)) static inline bool u7_spinlock_trylock(
    u7_spinlock* spinlock) {
  return !atomic_flag_test_and_set_explicit(&spinlock->flag,
                                            memory_order_acquire);
}

static inline void u7_spinlock_unlock(u7_spinlock* spinlock) {
  atomic_flag_clear_explicit(&spinlock->flag, memory_order_release);
}

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // __APPLE__

#endif  // U7_SPINLOCK_H_
