#ifndef U7_REFCOUNT_H_
#define U7_REFCOUNT_H_

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
// Inserts barriers to ensure that state written before this method returns
// false will be visible to a thread that just observed this method returning
// false.  Always returns false when the immortal bit is set.
static inline bool u7_refcount_decrement(const u7_refcount* refcount) {
  const int count =
      atomic_load_explicit((atomic_int*)&refcount->count, memory_order_relaxed);
  return (count != 1 &&
          atomic_fetch_sub_explicit((atomic_int*)&refcount->count, 1,
                                    memory_order_acq_rel) != 1);
}

#endif  // U7_REFCOUNT_H_
