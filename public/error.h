#ifndef U7_ERROR_H_
#define U7_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

struct u7_error_base;

typedef struct u7_error_base const u7_error;

typedef void (*u7_error_destroy_fn_t)(struct u7_error_base* self);

struct u7_error_base {
  // Reference counter for the error structure.
  //
  // This field much be used only by u7_error_acquire() and u7_error_release(),
  // The rest of the code MUST ignore value in this field.
  /* mutable */ _Atomic(int) ref_count;

  // A function for the error structure destruction.
  u7_error_destroy_fn_t destroy_fn;

  // Error category.
  //
  // The pointer address must uniquely identify the error category.
  const char* category;

  // Error code.
  //
  // The meaning of the error code depends on the category.
  int error_code;

  // Error message (string is zero terminated).
  const char* message;
  int message_length;

  // The next error in the chain.
  u7_error* cause;
};

// Acquires ownership of the error struct.
u7_error* u7_error_acquire(u7_error* self);

// Releases ownership of the error struct.
void u7_error_release(u7_error* self);

// A macro for return-if-error behaviour
#define U7_RETURN_IF_ERROR(call)     \
  do {                               \
    u7_error* u7_error_tmp = (call); \
    if (u7_error_tmp) {              \
      return u7_error_tmp;           \
    }                                \
  } while (0)

// Returns a pointer to an error category based on errno (see errno.h).
const char* u7_error_errno_category();

// Returns an out-of-memory error.
u7_error* u7_error_out_of_memory();

void u7_error_destroy_noop_fn(struct u7_error_base* self);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_ERROR_H_
