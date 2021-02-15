#include "@/public/error.h"

#include <stdio.h>

u7_error* fn() {
  U7_RETURN_IF_ERROR(u7_error_out_of_memory());
  return NULL;
}

int main() {
  u7_error* error = NULL;
  if ((error = fn())) {
    fprintf(stderr, "%s(%d): %*s\n", error->category, error->error_code,
            error->message_length, error->message);
    u7_error_release(error);
    return 0;
  }
  return -1;
}
