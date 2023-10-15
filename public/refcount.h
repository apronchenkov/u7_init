#ifndef U7_REFCOUNT_H_
#define U7_REFCOUNT_H_

#ifndef __cplusplus

#include <stdatomic.h>
#include <stdbool.h>

// A compact type for reference counting.
//
// The implementation is based on the ideas in
// absl/strings/internal/cord_internal.h
typedef struct {
  /*mutable*/ atomic_int count;
} u7_refcount;

#define U7_REFCOUNT_INIT \
  { .count = 1 }

// Resets the reference count to 1.
static inline void u7_refcount_init(u7_refcount* refcount) {
  refcount->count = 1;
}

// Increments the reference count. Imposes no memory ordering.
static inline void u7_refcount_increment(const u7_refcount* refcount) {
  atomic_fetch_add_explicit((atomic_int*)&refcount->count, 1,
                            memory_order_relaxed);
}

// Decrements the reference count.
//
// Returns false if there are no references outstanding; true otherwise.
__attribute__((__warn_unused_result__)) static inline bool
u7_refcount_decrement(const u7_refcount* refcount) {
  return (atomic_fetch_sub_explicit((atomic_int*)&refcount->count, 1,
                                    memory_order_acq_rel) != 1);
}

// Decrements the reference count; this function is more efficient if you expect
// exclusive ownership.
//
// Returns false if there are no references outstanding; true otherwise.
//
__attribute__((__warn_unused_result__)) static inline bool
u7_refcount_skewed_decrement(const u7_refcount* refcount) {
  const int count =
      atomic_load_explicit((atomic_int*)&refcount->count, memory_order_relaxed);
  return (count != 1 &&
          atomic_fetch_sub_explicit((atomic_int*)&refcount->count, 1,
                                    memory_order_acq_rel) != 1);
}

#else  // __cplusplus

#include <atomic>

extern "C" {

// A compact type for reference counting.
//
// The implementation is based on the ideas in
// absl/strings/internal/cord_internal.h
typedef struct {
  mutable std::atomic_int count;
} u7_refcount;

#define U7_REFCOUNT_INIT \
  { .count = 1 }

// Resets the reference count to 1.
static inline void u7_refcount_init(u7_refcount* refcount) {
  refcount->count = 1;
}

// Increments the reference count. Imposes no memory ordering.
static inline void u7_refcount_increment(const u7_refcount* refcount) {
  refcount->count.fetch_add(1, std::memory_order_relaxed);
}

// Decrements the reference count.
//
// Returns false if there are no references outstanding; true otherwise.
__attribute__((__warn_unused_result__)) static inline bool
u7_refcount_decrement(const u7_refcount* refcount) {
  return (refcount->count.fetch_sub(1, std::memory_order_acq_rel) != 1);
}

// Decrements the reference count; this function is more efficient if you expect
// exclusive ownership.
//
// Returns false if there are no references outstanding; true otherwise.
//
__attribute__((__warn_unused_result__)) static inline bool
u7_refcount_skewed_decrement(const u7_refcount* refcount) {
  const int count = refcount->count.load(std::memory_order_relaxed);
  return (count != 1 &&
          refcount->count.fetch_sub(1, std::memory_order_acq_rel) != 1);
}

}  // extern "C"

#endif  // __cplusplus

#endif  // U7_REFCOUNT_H_
