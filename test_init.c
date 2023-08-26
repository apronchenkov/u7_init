#include "@/public/init.h"

#include <github.com/apronchenkov/u7_init/public/error.h>
#include <stdio.h>

int main(void) {
  u7_error error = u7_init();
  if (error.error_code) {
    fprintf(stderr, "%" U7_ERROR_FMT "\n", U7_ERROR_FMT_PARAMS(error));
    u7_error_release(error);
    return -1;
  }
  return 0;
}
