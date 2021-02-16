#include "@/public/error.h"

#include <errno.h>
#include <stdio.h>

u7_error fn_ok() { return u7_ok(); }

u7_error fn_err() {
  U7_RETURN_IF_ERROR(
      u7_errorf(u7_errno_category(), EINVAL, "%s", "Hello, World!"));
  return u7_ok();
}

int main() {
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
  return 0;
}
