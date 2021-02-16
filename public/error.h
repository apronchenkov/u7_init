#ifndef U7_ERROR_H_
#define U7_ERROR_H_

#include <stdarg.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

struct u7_error_category;
struct u7_error_payload;

typedef struct {
  // Error code within the category.
  //
  // NOTE: code=0 is reserved for OK.
  int error_code;

  // Error category.
  //
  // The pointer address must uniquely identify the error category.
  struct u7_error_payload const* payload;
} u7_error;

// Additional information about an error instance.
struct u7_error_payload {
  // Reference counter.
  //
  // This field much be used only by u7_error_acquire() and u7_error_release(),
  // The rest of the code MUST ignore this field.
  /* mutable */ _Atomic(int) ref_count;

  // Error category.
  //
  // The pointer address must uniquely identify the error category.
  struct u7_error_category const* category;

  // Error message (string is zero terminated).
  const char* message;
  int message_length;

  // The next error in the chain.
  u7_error cause;
};

// NOTE: Message should be deallocatable with free().
typedef struct u7_error_payload const* (*u7_error_category_make_payload_fn_t)(
    struct u7_error_category const* self, char* message, int message_length,
    u7_error cause);

typedef void (*u7_error_category_destroy_payload_fn_t)(
    struct u7_error_category const* self, struct u7_error_payload* payload);

struct u7_error_category {
  // Category name.
  const char* name;

  // Factory function for payload of this category.
  u7_error_category_make_payload_fn_t make_payload_fn;

  // Destructor for payloads of this category.
  u7_error_category_destroy_payload_fn_t destroy_payload_fn;
};

// Returns a pointer to an ok category.
static inline u7_error u7_ok() {
  u7_error result = {0};
  return result;
}

// Acquires ownership of the error struct.
u7_error u7_error_acquire(u7_error self);

// Releases ownership of the error struct.
void u7_error_release(u7_error self);

// Returns error's category name.
static inline const char* u7_error_category_name(u7_error error) {
  return error.error_code ? error.payload->category->name : "OK";
}

// Returns error message.
static inline const char* u7_error_message(u7_error error) {
  return error.error_code ? error.payload->message : "ok";
}

// Returns error message length.
static inline int u7_error_message_length(u7_error error) {
  return error.error_code ? error.payload->message_length : 2;
}

// Returns an error object with given category.
u7_error u7_verrorf_with_cause(struct u7_error_category const* category,
                               int error_code, u7_error cause,
                               const char* format, va_list arg);

u7_error u7_errorf_with_cause(struct u7_error_category const* category,
                              int error_code, u7_error cause,
                              const char* format, ...)
    __attribute__((format(printf, 4, 5)));

u7_error u7_verrorf(struct u7_error_category const* category, int error_code,
                    const char* format, va_list arg);

u7_error u7_errorf(struct u7_error_category const* category, int error_code,
                   const char* format, ...)
    __attribute__((format(printf, 3, 4)));

// A macro for return-if-error behaviour
#define U7_RETURN_IF_ERROR(call)    \
  do {                              \
    u7_error u7_error_tmp = (call); \
    if (u7_error_tmp.error_code) {  \
      return u7_error_tmp;          \
    }                               \
  } while (0)

#define U7_ERROR_FMT "s(%d): %*s"
#define U7_ERROR_FMT_PARAMS(e)                                           \
  u7_error_category_name(e), (e).error_code, u7_error_message_length(e), \
      u7_error_message(e)

// Returns a pointer to an error category based on errno (see errno.h).
struct u7_error_category const* u7_errno_category();

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_ERROR_H_
