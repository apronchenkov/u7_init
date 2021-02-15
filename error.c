#include "@/public/error.h"

#include <errno.h>
#include <stdatomic.h>

u7_error* u7_error_acquire(u7_error* self) {
  if (self) {
    atomic_fetch_add_explicit(&((struct u7_error_base*)self)->ref_count, 1,
                              memory_order_relaxed);
  }
  return self;
}

void u7_error_release(u7_error* self) {
  // Avoid reccursion.
  while (self && 1 == atomic_fetch_add_explicit(
                          &((struct u7_error_base*)self)->ref_count, -1,
                          memory_order_relaxed)) {
    u7_error* const self_cause = self->cause;
    self->destroy_fn((struct u7_error_base*)self);
    self = self_cause;
  }
}

const char* u7_error_errno_category() {
  static char const* const result = "Errno";
  return result;
}

static void u7_error_destry_noop(struct u7_error_base* self) { (void)self; }

u7_error* u7_error_out_of_memory() {
#define u7_error_out_of_memory_message "out of memory"
  static struct u7_error_base result = {
      .ref_count = 1,
      .destroy_fn = &u7_error_destry_noop,
      .error_code = ENOMEM,
      .message = u7_error_out_of_memory_message,
      .message_length = sizeof(u7_error_out_of_memory_message) - 1,
  };
#undef u7_error_out_of_memory_message

  if (!result.category) {  // Cannot be statically initialized.
    result.category = u7_error_errno_category();
  }

  return u7_error_acquire(&result);
}
