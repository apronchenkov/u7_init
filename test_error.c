#include "@/public/error.h"
#include "@/public/testing.h"

#include <errno.h>

static u7_error fn_ok(void) { return u7_ok(); }

static u7_error fn_err(void) {
  U7_RETURN_IF_ERROR(
      u7_errorf(u7_errno_category(), EINVAL, "%s", "Hello, World!"));
  return u7_ok();
}

static u7_error gn_err(void) {
  return u7_errorf_with_cause(u7_errno_category(), EINVAL, fn_err(), "%s",
                              "foo.bar");
}

U7_TEST(test_ok) {
  u7_error error = fn_ok();
  U7_ASSERT_OK(error);
  U7_ASSERT_EQ(u7_error_category_name(error), "OK");
  U7_ASSERT_EQ(u7_error_message(error), "ok");
  u7_error_release(error);
}

U7_TEST(test_error) {
  u7_error error = fn_err();
  U7_ASSERT_ERROR_CODE(error, EINVAL);
  U7_ASSERT_EQ(u7_error_category_name(error), "Errno");
  U7_ASSERT_EQ(u7_error_message(error), "Hello, World!");
  u7_error_release(error);
}

U7_TEST(test_error_cause) {
  u7_error error = gn_err();
  U7_ASSERT_ERROR_CODE(error, EINVAL);
  U7_ASSERT_EQ(u7_error_message(error), "foo.bar");
  U7_ASSERT(error.payload != NULL);
  U7_ASSERT_ERROR_CODE(error.payload->cause, EINVAL);
  U7_ASSERT_EQ(u7_error_message(error.payload->cause), "Hello, World!");
  u7_error_release(error);
}

U7_TEST(test_empty_message_preserves_cause) {
  u7_error cause = u7_errnof(EINVAL, "%s", "inner");
  u7_error error = u7_errorf_with_cause(u7_errno_category(), EIO, cause, "");
  U7_ASSERT_ERROR_CODE(error, EIO);
  U7_ASSERT_EQ(u7_error_message(error), "");
  U7_ASSERT_EQ(u7_error_message_length(error), 0);
  U7_ASSERT(error.payload != NULL);
  U7_ASSERT_ERROR_CODE(error.payload->cause, EINVAL);
  U7_ASSERT_EQ(u7_error_message(error.payload->cause), "inner");
  u7_error_release(error);
}

U7_TEST(test_string_precision) {
  u7_error error = u7_errnof(EINVAL, "%.*s", 3, "abcdef");
  U7_ASSERT_EQ(u7_error_message(error), "abc");
  U7_ASSERT_EQ(u7_error_message_length(error), 3);
  u7_error_release(error);

  error = u7_errnof(EINVAL, "%.*s", 16, "abc");
  U7_ASSERT_EQ(u7_error_message(error), "abc");
  U7_ASSERT_EQ(u7_error_message_length(error), 3);
  u7_error_release(error);

  error = u7_errnof(EINVAL, "%.*s", -1, "abcdef");
  U7_ASSERT_EQ(u7_error_message(error), "abcdef");
  U7_ASSERT_EQ(u7_error_message_length(error), 6);
  u7_error_release(error);

  error = u7_errnof(EINVAL, "%.*s", 0, "abcdef");
  U7_ASSERT_EQ(u7_error_message(error), "");
  U7_ASSERT_EQ(u7_error_message_length(error), 0);
  u7_error_release(error);

  const char unterminated[] = {'a', 'b', 'c'};
  error = u7_errnof(EINVAL, "%.*s", 3, unterminated);
  U7_ASSERT_EQ(u7_error_message(error), "abc");
  U7_ASSERT_EQ(u7_error_message_length(error), 3);
  u7_error_release(error);
}

int main(int argc, char** argv) {
  return u7_testing_run_registered(argc, argv);
}
