#ifndef U7_ERROR_H_
#define U7_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

struct u7_error;

typedef void (*u7_error_destroy_fn_t)(struct u7_error* self);

struct u7_error {
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
  int message_size;

  // The next error in the chain.
  struct u7_error const* cause;
};

// Acquires ownership of the error struct.
struct u7_error const* u7_error_acquire(struct u7_error const* self);

// Releases ownership of the error struct.
void u7_error_release(struct u7_error const* self);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_ERROR_H_
