#include "@/public/spinlock.h"

#include <sched.h>
#include <time.h>

#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
#include <intrin.h>
#define YALOG_SPINLOCK_ASM_VOLATILE_PAUSE() ::_mm_pause()
#elif defined(__i386__) || defined(__x86_64__) || defined(_M_X64)
#define YALOG_SPINLOCK_ASM_VOLATILE_PAUSE() __asm__ volatile("pause")
#elif defined(__arm__) || defined(__aarch64__)
#define YALOG_SPINLOCK_ASM_VOLATILE_PAUSE() __asm__ volatile("yield")
#endif

int u7_spinlock_internal_yield_k(int k) {
  if (k < 4) {
    return k + 1;
  } else if (k < 16) {
    YALOG_SPINLOCK_ASM_VOLATILE_PAUSE();
    return k + 1;
  } else if (k < 32) {
    if (k & 1) {
      sched_yield();
    } else {
      struct timespec ts = {.tv_sec = 0, .tv_nsec = 1000};
      nanosleep(&ts, 0);
    }
    return k + 1;
  } else {
    struct timespec ts = {.tv_sec = 0, .tv_nsec = 500000};
    nanosleep(&ts, 0);
    return k;
  }
}
