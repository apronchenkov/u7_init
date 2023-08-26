#include "@/public/error.h"

#include "@/public/refcount.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

u7_error u7_error_acquire(u7_error self) {
  if (self.payload) {
    u7_refcount_increment(&self.payload->refcount);
  }
  return self;
}

void u7_error_release(u7_error self) {
  // Avoid reccursion.
  struct u7_error_payload* payload = (struct u7_error_payload*)self.payload;
  while (payload && !u7_refcount_decrement(&payload->refcount)) {
    struct u7_error_payload* const cause_payload =
        (struct u7_error_payload*)payload->cause.payload;
    if (payload->dispose_fn != NULL) {
      payload->dispose_fn(payload);
    }
    payload = cause_payload;
  }
}

void u7_error_clear(u7_error* self) {
  u7_error_release(*self);
  self->error_code = 0;
  self->payload = NULL;
}

// Returns an error object with given category.
u7_error u7_verrorf_with_cause(struct u7_error_category const* category,
                               int error_code, u7_error cause,
                               const char* format, va_list arg) {
  const int kN = 128;
  u7_error result;
  int message_length = -1;
  char* message = NULL;
  {  // Sanity check.
    assert(error_code != 0);
    if (error_code == 0) {
      u7_error_release(cause);
      return u7_ok();
    }
  }
  {
    // Fast case: format=""
    if (format[0] == '\0') {
      goto ret;
    }
  }
  {
    // Fast case: format="%s"
    if (format[0] == '%' && format[1] == 's' && format[2] == '\0') {
      message = strdup(va_arg(arg, const char*));
      if (message) {
        message_length = strlen(message);
      }
      goto ret;
    }
  }
  {
    // Fast case: format="%.*s"
    if (format[0] == '%' && format[1] == '.' && format[2] == '*' &&
        format[3] == 's' && format[4] == '\0') {
      message_length = va_arg(arg, int);
      message = strndup(va_arg(arg, const char*), message_length);
      goto ret;
    }
  }
  {
    // Short message.
    message = (char*)malloc(kN);
    if (message == NULL) {
      goto fail;
    }
    va_list arg_copy;
    va_copy(arg_copy, arg);
    int n = vsnprintf(message, kN, format, arg_copy);
    va_end(arg_copy);
    assert(n >= 0);
    if (n < 0) {
      goto fail;
    }
    message_length = n;
    if (message_length < kN) {
      goto ret;
    }
  }
  {
    // Long message.
    char* tmp = (char*)realloc(message, message_length + 1);
    if (!tmp) {
      goto fail;
    }
    message = tmp;
    int n = vsnprintf(message, message_length + 1, format, arg);
    assert(n == message_length);
    if (n != message_length) {
      goto fail;
    }
    goto ret;
  }
fail:
  free(message);
  message = NULL;
  message_length = -1;
ret:
  result.error_code = error_code;
  result.payload =
      category->make_payload_fn(category, message, message_length, cause);
  return result;
}

u7_error u7_errorf_with_cause(struct u7_error_category const* category,
                              int error_code, u7_error cause,
                              const char* format, ...) {
  va_list arg;
  va_start(arg, format);
  u7_error error =
      u7_verrorf_with_cause(category, error_code, cause, format, arg);
  va_end(arg);
  return error;
}

u7_error u7_verrorf(struct u7_error_category const* category, int error_code,
                    const char* format, va_list arg) {
  return u7_verrorf_with_cause(category, error_code, u7_ok(), format, arg);
}

u7_error u7_errorf(struct u7_error_category const* category, int error_code,
                   const char* format, ...) {
  va_list arg;
  va_start(arg, format);
  u7_error error =
      u7_verrorf_with_cause(category, error_code, u7_ok(), format, arg);
  va_end(arg);
  return error;
}

static struct u7_error_payload const* u7_errno_category_make_payload_fn(
    struct u7_error_category const* self, char* message, int message_length,
    u7_error cause);

static struct u7_error_category u7_errno_category_static_category = {
    .name = "Errno",
    .make_payload_fn = &u7_errno_category_make_payload_fn,
};

static const char u7_errno_category_static_fallback_message[] =
    "<u7_error:fail>";

static void u7_errno_category_payload_dispose_fn(
    struct u7_error_payload* self) {
  assert(self->category == &u7_errno_category_static_category);
  free((char*)self->message);
  free(self);
}

static struct u7_error_payload u7_errno_category_static_fallback_payload = {
    .refcount = U7_REFCOUNT_INIT,
    .dispose_fn = NULL,
    .category = &u7_errno_category_static_category,
    .message = u7_errno_category_static_fallback_message,
    .message_length = sizeof(u7_errno_category_static_fallback_message) - 1,
    .cause = {.error_code = 0, .payload = NULL},
};

static struct u7_error_payload const* u7_errno_category_make_payload_fn(
    struct u7_error_category const* self, char* message, int message_length,
    u7_error cause) {
  (void)self;
  assert(self == &u7_errno_category_static_category);
  if (message_length < 0) {
    free(message);
    u7_error_release(cause);
    u7_refcount_increment(&u7_errno_category_static_fallback_payload.refcount);
    return &u7_errno_category_static_fallback_payload;
  }
  struct u7_error_payload* result =
      (struct u7_error_payload*)malloc(sizeof(struct u7_error_payload));
  if (result == NULL) {
    free(message);
    u7_error_release(cause);
    u7_refcount_increment(&u7_errno_category_static_fallback_payload.refcount);
    return &u7_errno_category_static_fallback_payload;
  }
  u7_refcount_init(&result->refcount);
  result->dispose_fn = &u7_errno_category_payload_dispose_fn;
  result->category = &u7_errno_category_static_category;
  result->message = message;
  result->message_length = message_length;
  result->cause = cause;
  return result;
}

struct u7_error_category const* u7_errno_category(void) {
  return &u7_errno_category_static_category;
}

u7_error u7_errnof(int errno_code, const char* format, ...) {
  va_list arg;
  va_start(arg, format);
  u7_error error = u7_verrorf_with_cause(u7_errno_category(), errno_code,
                                         u7_ok(), format, arg);
  va_end(arg);
  return error;
}
