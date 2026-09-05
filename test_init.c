#include "@/public/init.h"
#include "@/public/testing.h"

#include <errno.h>

extern char u7_testing_initializer_order[4];

static u7_error init_noop(void) { return u7_ok(); }

U7_TEST(test_initializer_lifecycle_and_validation) {
  u7_error error = u7_init();
  U7_ASSERT_OK(error);
  U7_ASSERT_EQ(u7_testing_initializer_order, "bfz");
  u7_error_release(error);

  error = u7_init();
  U7_ASSERT_OK(error);
  u7_error_release(error);

  static struct u7_initializer duplicate_a = {
      .precedence = 0,
      .name = "duplicate",
      .init_fn = &init_noop,
  };
  static struct u7_initializer duplicate_b = {
      .precedence = 1,
      .name = "duplicate",
      .init_fn = &init_noop,
  };
  u7_initializer_register(&duplicate_a);
  u7_initializer_register(&duplicate_b);
  error = u7_init();
  U7_ASSERT_ERROR_CODE(error, EINVAL);
  U7_ASSERT_EQ(u7_error_message(error),
               "u7_init: non-unique initializer name: duplicate");
  u7_error_release(error);
}

int main(int argc, char** argv) {
  return u7_testing_run_registered(argc, argv);
}
