#include "@/public/spinlock.h"
#include "@/public/testing.h"

#include <pthread.h>
#include <stddef.h>

struct counter {
  u7_spinlock lock;
  size_t value;
};

struct worker {
  struct counter* counter;
  size_t iterations;
};

static void* increment(void* data) {
  struct worker* worker = data;
  for (size_t i = 0; i < worker->iterations; ++i) {
    u7_spinlock_lock(&worker->counter->lock);
    worker->counter->value += 1;
    u7_spinlock_unlock(&worker->counter->lock);
  }
  return NULL;
}

U7_TEST(test_init_lock_unlock_and_trylock) {
  u7_spinlock lock;
  u7_spinlock_init(&lock);
  U7_ASSERT(u7_spinlock_trylock(&lock));
  U7_ASSERT(!u7_spinlock_trylock(&lock));
  u7_spinlock_unlock(&lock);
  U7_ASSERT(u7_spinlock_trylock(&lock));
  u7_spinlock_unlock(&lock);
}

U7_TEST(test_serializes_multiple_threads) {
  enum { WORKERS = 4, ITERATIONS = 10000 };
  struct counter counter = {.lock = U7_SPINLOCK_INIT};
  struct worker worker = {
      .counter = &counter,
      .iterations = ITERATIONS,
  };
  pthread_t threads[WORKERS];
  for (size_t i = 0; i < WORKERS; ++i) {
    U7_ASSERT_EQ(pthread_create(&threads[i], NULL, increment, &worker), 0);
  }
  for (size_t i = 0; i < WORKERS; ++i) {
    U7_ASSERT_EQ(pthread_join(threads[i], NULL), 0);
  }
  U7_ASSERT_EQ(counter.value, WORKERS * ITERATIONS);
}

int main(int argc, char** argv) {
  return u7_testing_run_registered(argc, argv);
}
