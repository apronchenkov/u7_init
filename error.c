#include "@/public/error.h"

#include <stdatomic.h>

struct u7_error const* u7_error_acquire(struct u7_error const* self) {
  if (self) {
    atomic_fetch_add_explicit(&((struct u7_error*)self)->ref_count, 1,
                              memory_order_relaxed);
  }
  return self;
}

void u7_error_release(struct u7_error const* self) {
  // Avoid reccursion.
  while (self &&
         1 == atomic_fetch_add_explicit(&((struct u7_error*)self)->ref_count,
                                        -1, memory_order_relaxed)) {
    struct u7_error const* const self_cause = self->cause;
    self->destroy_fn((struct u7_error*)self);
    self = self_cause;
  }
}
