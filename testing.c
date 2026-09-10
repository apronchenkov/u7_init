#include "@/public/testing.h"

#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* u7_testing_current_name;
static struct u7_testing_case* u7_testing_registered_tests;

void u7_testing_register(struct u7_testing_case* test) {
  test->next = u7_testing_registered_tests;
  u7_testing_registered_tests = test;
}

[[noreturn]] static void u7_testing_fail(const char* file, int line,
                                         const char* message) {
  fprintf(stderr, "%s:%d: %s: %s\n", file, line,
          u7_testing_current_name != NULL ? u7_testing_current_name : "test",
          message);
  abort();
}

[[noreturn]] void u7_testing_assert_fail(const char* file, int line,
                                         const char* format, ...) {
  char message[1024];
  va_list args;
  va_start(args, format);
  (void)vsnprintf(message, sizeof(message), format, args);
  va_end(args);
  u7_testing_fail(file, line, message);
}

// Renders `value` as text for a failure message. Never fails: an unknown
// kind (which should not happen) renders as "?" rather than crashing the
// crash-reporting path.
static void u7_testing_format_value(char* buf, size_t size,
                                    struct u7_testing_value value) {
  switch (value.kind) {
    case U7_TESTING_KIND_I64:
      snprintf(buf, size, "%" PRId64, value.as.i64);
      return;
    case U7_TESTING_KIND_U64:
      snprintf(buf, size, "%" PRIu64, value.as.u64);
      return;
    case U7_TESTING_KIND_F64:
      snprintf(buf, size, "%.17g", value.as.f64);
      return;
    case U7_TESTING_KIND_STR:
      if (value.as.str == NULL) {
        snprintf(buf, size, "(null)");
      } else {
        snprintf(buf, size, "\"%s\"", value.as.str);
      }
      return;
  }
  snprintf(buf, size, "?");
}

static const char* u7_testing_kind_name(enum u7_testing_kind kind) {
  switch (kind) {
    case U7_TESTING_KIND_I64:
      return "a signed integer";
    case U7_TESTING_KIND_U64:
      return "an unsigned integer";
    case U7_TESTING_KIND_F64:
      return "a floating-point number";
    case U7_TESTING_KIND_STR:
      return "a string";
  }
  return "a value of unknown kind";
}

// Compares two non-string values, handling every signed/unsigned pairing
// explicitly instead of relying on C's usual arithmetic conversions (which
// would silently convert a negative signed value to a huge unsigned one).
static bool u7_testing_values_equal(struct u7_testing_value actual,
                                    struct u7_testing_value expected) {
  if (actual.kind == U7_TESTING_KIND_I64 &&
      expected.kind == U7_TESTING_KIND_I64) {
    return actual.as.i64 == expected.as.i64;
  }
  if (actual.kind == U7_TESTING_KIND_U64 &&
      expected.kind == U7_TESTING_KIND_U64) {
    return actual.as.u64 == expected.as.u64;
  }
  if (actual.kind == U7_TESTING_KIND_I64 &&
      expected.kind == U7_TESTING_KIND_U64) {
    return actual.as.i64 >= 0 && (uint64_t)actual.as.i64 == expected.as.u64;
  }
  if (actual.kind == U7_TESTING_KIND_U64 &&
      expected.kind == U7_TESTING_KIND_I64) {
    return expected.as.i64 >= 0 && actual.as.u64 == (uint64_t)expected.as.i64;
  }
  // At least one operand is floating-point: compare in double, same as
  // comparing any two of {int64_t, uint64_t, double} directly would.
  double actual_f64 = actual.kind == U7_TESTING_KIND_I64 ? (double)actual.as.i64
                      : actual.kind == U7_TESTING_KIND_U64
                          ? (double)actual.as.u64
                          : actual.as.f64;
  double expected_f64 =
      expected.kind == U7_TESTING_KIND_I64   ? (double)expected.as.i64
      : expected.kind == U7_TESTING_KIND_U64 ? (double)expected.as.u64
                                             : expected.as.f64;
  return actual_f64 == expected_f64;
}

void u7_testing_assert_eq(struct u7_testing_value actual,
                          struct u7_testing_value expected,
                          const char* actual_expression,
                          const char* expected_expression, const char* file,
                          int line) {
  const bool actual_is_str = actual.kind == U7_TESTING_KIND_STR;
  const bool expected_is_str = expected.kind == U7_TESTING_KIND_STR;
  if (actual_is_str != expected_is_str) {
    u7_testing_assert_fail(file, line, "cannot compare %s (%s) with %s (%s)",
                           actual_expression, u7_testing_kind_name(actual.kind),
                           expected_expression,
                           u7_testing_kind_name(expected.kind));
  }
  const bool equal =
      actual_is_str ? (actual.as.str == NULL) == (expected.as.str == NULL) &&
                          (actual.as.str == NULL ||
                           strcmp(actual.as.str, expected.as.str) == 0)
                    : u7_testing_values_equal(actual, expected);
  if (!equal) {
    char actual_buf[256];
    char expected_buf[256];
    u7_testing_format_value(actual_buf, sizeof(actual_buf), actual);
    u7_testing_format_value(expected_buf, sizeof(expected_buf), expected);
    u7_testing_assert_fail(file, line, "expected %s == %s; actual: %s != %s",
                           actual_expression, expected_expression, actual_buf,
                           expected_buf);
  }
}

int u7_testing_run(int argc, char** argv, struct u7_testing_case const* tests,
                   size_t tests_size) {
  const char* filter = NULL;
  bool list = false;
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "--list") == 0) {
      list = true;
    } else if (strcmp(argv[i], "--filter") == 0 && i + 1 < argc) {
      filter = argv[++i];
    } else if (strncmp(argv[i], "--filter=", 9) == 0) {
      filter = argv[i] + 9;
    } else {
      fprintf(stderr, "u7_testing: unknown argument: %s\n", argv[i]);
      return 2;
    }
  }
  size_t selected = 0;
  for (size_t i = 0; i < tests_size; ++i) {
    if (filter != NULL && strstr(tests[i].name, filter) == NULL) {
      continue;
    }
    selected += 1;
    if (list) {
      puts(tests[i].name);
      continue;
    }
    u7_testing_current_name = tests[i].name;
    tests[i].fn();
    printf("[ PASS ] %s\n", tests[i].name);
    (void)fflush(stdout);
  }
  u7_testing_current_name = NULL;
  if (selected == 0 && filter != NULL) {
    fprintf(stderr, "u7_testing: no tests match filter: %s\n", filter);
    return 2;
  }
  return 0;
}

static int u7_testing_compare_cases(const void* lhs, const void* rhs) {
  const struct u7_testing_case* const lhs_case =
      *(const struct u7_testing_case* const*)lhs;
  const struct u7_testing_case* const rhs_case =
      *(const struct u7_testing_case* const*)rhs;
  return strcmp(lhs_case->name, rhs_case->name);
}

int u7_testing_run_registered(int argc, char** argv) {
  size_t count = 0;
  for (struct u7_testing_case* test = u7_testing_registered_tests; test != NULL;
       test = test->next) {
    count += 1;
  }
  if (count == 0) {
    fputs("u7_testing: no tests registered\n", stderr);
    return 2;
  }
  struct u7_testing_case** tests = malloc(count * sizeof(tests[0]));
  if (tests == NULL) {
    fputs("u7_testing: cannot allocate registered test index\n", stderr);
    return 2;
  }
  size_t index = 0;
  for (struct u7_testing_case* test = u7_testing_registered_tests; test != NULL;
       test = test->next) {
    tests[index++] = test;
  }
  qsort(tests, count, sizeof(tests[0]), u7_testing_compare_cases);
  for (size_t i = 1; i < count; ++i) {
    if (strcmp(tests[i - 1]->name, tests[i]->name) == 0) {
      fprintf(stderr, "u7_testing: duplicate test name: %s\n", tests[i]->name);
      free(tests);
      return 2;
    }
  }
  // The pointer array has the same field order as u7_testing_case only by
  // accident, so copy the sorted records before using the explicit runner.
  struct u7_testing_case* sorted = malloc(count * sizeof(sorted[0]));
  if (sorted == NULL) {
    free(tests);
    fputs("u7_testing: cannot allocate sorted test list\n", stderr);
    return 2;
  }
  for (size_t i = 0; i < count; ++i) {
    sorted[i] = *tests[i];
    sorted[i].next = NULL;
  }
  free(tests);
  const int result = u7_testing_run(argc, argv, sorted, count);
  free(sorted);
  return result;
}
