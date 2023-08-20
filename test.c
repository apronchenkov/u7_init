#include "@/public/error.h"

#include <errno.h>
#include <stdio.h>

u7_error fn_ok(void) { return u7_ok(); }

u7_error fn_err(void) {
  U7_RETURN_IF_ERROR(
      u7_errorf(u7_errno_category(), EINVAL, "%s", "Hello, World!"));
  return u7_ok();
}

u7_error gn_err(void) {
  return u7_errorf_with_cause(u7_errno_category(), EINVAL, fn_err(), "%s",
                              "foo.bar");
}

int main(void) {
  u7_error error;
  if ((error = fn_ok()).error_code) {
    fprintf(stderr, "%" U7_ERROR_FMT "\n", U7_ERROR_FMT_PARAMS(error));
    u7_error_release(error);
    return -1;
  } else {
    fprintf(stderr, "%" U7_ERROR_FMT "\n", U7_ERROR_FMT_PARAMS(error));
    u7_error_release(error);
  }

  if ((error = fn_err()).error_code) {
    fprintf(stderr, "%" U7_ERROR_FMT "\n", U7_ERROR_FMT_PARAMS(error));
    u7_error_release(error);
  } else {
    fprintf(stderr, "%" U7_ERROR_FMT "\n", U7_ERROR_FMT_PARAMS(error));
    u7_error_release(error);
    return -1;
  }

  if ((error = gn_err()).error_code) {
    fprintf(stderr, "%" U7_ERROR_FMT "\n", U7_ERROR_FMT_PARAMS(error));
    u7_error_release(error);
  } else {
    fprintf(stderr, "%" U7_ERROR_FMT "\n", U7_ERROR_FMT_PARAMS(error));
    u7_error_release(error);
    return -1;
  }

  return 0;
}
