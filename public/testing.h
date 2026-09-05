#ifndef U7_TESTING_H_
#define U7_TESTING_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*u7_testing_fn_t)(void);

struct u7_testing_case {
  const char* name;
  u7_testing_fn_t fn;
  struct u7_testing_case* next;
};

// Runs explicitly registered tests. Supported arguments are --list and
// --filter <substring>. Returns nonzero for invalid command-line arguments.
int u7_testing_run(int argc, char** argv, struct u7_testing_case const* tests,
                   size_t tests_size);

// Registers and runs tests declared with U7_TEST. Registration order does not
// affect execution order; registered tests run lexicographically by name.
//
// NOTE: All tests in a binary run sequentially in the same process, in
// lexicographic order by name, and share whatever process-global state your
// code under test has. A test that mutates global state (as test_init.c's
// initializer registry test does) must account for tests before and after it
// in name order; consider resetting or fully consuming shared state within
// the test that touches it.
void u7_testing_register(struct u7_testing_case* test);
int u7_testing_run_registered(int argc, char** argv);

_Noreturn void u7_testing_assert_fail(const char* file, int line,
                                      const char* format, ...)
    __attribute__((format(printf, 3, 4)));

// U7_ASSERT_EQ.
//
// Type erasure for U7_ASSERT_EQ: a value is tagged with its comparison kind
// so testing.c can compare mixed signed/unsigned/float pairs correctly
// instead of relying on C's usual arithmetic conversions (which quietly
// convert a negative signed value to a huge unsigned one). This machinery is
// not meant to be used directly.
enum u7_testing_kind {
  U7_TESTING_KIND_I64,
  U7_TESTING_KIND_U64,
  U7_TESTING_KIND_F64,
  U7_TESTING_KIND_STR,
};

struct u7_testing_value {
  enum u7_testing_kind kind;
  union {
    int64_t i64;
    uint64_t u64;
    double f64;
    const char* str;
  } as;
};

static inline struct u7_testing_value u7_testing_value_i64(int64_t value) {
  return (struct u7_testing_value){.kind = U7_TESTING_KIND_I64,
                                   .as = {.i64 = value}};
}

static inline struct u7_testing_value u7_testing_value_u64(uint64_t value) {
  return (struct u7_testing_value){.kind = U7_TESTING_KIND_U64,
                                   .as = {.u64 = value}};
}

static inline struct u7_testing_value u7_testing_value_f64(double value) {
  return (struct u7_testing_value){.kind = U7_TESTING_KIND_F64,
                                   .as = {.f64 = value}};
}

static inline struct u7_testing_value u7_testing_value_str(const char* value) {
  return (struct u7_testing_value){.kind = U7_TESTING_KIND_STR,
                                   .as = {.str = value}};
}

// Maps `value_` to a struct u7_testing_value for U7_ASSERT_EQ. Supports every
// standard integer type, float, double, and char*/const char*.
#define U7_TESTING_VALUE(value_)                \
  _Generic((value_),                            \
      char: u7_testing_value_i64,               \
      signed char: u7_testing_value_i64,        \
      short: u7_testing_value_i64,              \
      int: u7_testing_value_i64,                \
      long: u7_testing_value_i64,               \
      long long: u7_testing_value_i64,          \
      unsigned char: u7_testing_value_u64,      \
      unsigned short: u7_testing_value_u64,     \
      unsigned int: u7_testing_value_u64,       \
      unsigned long: u7_testing_value_u64,      \
      unsigned long long: u7_testing_value_u64, \
      float: u7_testing_value_f64,              \
      double: u7_testing_value_f64,             \
      char*: u7_testing_value_str,              \
      const char*: u7_testing_value_str)(value_)

// Compares two typed values for equality; aborts with a diagnostic if they
// differ, or if they cannot be compared at all (a string against a number).
// The comparison table lives in testing.c, not here.
void u7_testing_assert_eq(struct u7_testing_value actual,
                          struct u7_testing_value expected,
                          const char* actual_expression,
                          const char* expected_expression, const char* file,
                          int line);

#define U7_ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#define U7_TESTING_CASE(fn_) {.name = #fn_, .fn = (fn_)}

// Defines a static test and registers it before main. Test sources must be
// linked directly into the test executable; archive members that contain only
// registered tests may be discarded by the linker.
#define U7_TEST(name_) U7_TESTING_DEFINE(__COUNTER__, name_)
#define U7_TESTING_DEFINE(counter_, name_) \
  U7_TESTING_DEFINE_IMPL(counter_, name_)
#define U7_TESTING_DEFINE_IMPL(counter_, name_)                               \
  static void name_(void);                                                    \
  static struct u7_testing_case u7_testing_record_##counter_ = {              \
      .name = #name_, .fn = name_};                                           \
  __attribute__((constructor)) static void u7_testing_ctor_##counter_(void) { \
    u7_testing_register(&u7_testing_record_##counter_);                       \
  }                                                                           \
  static void name_(void)

#define U7_ASSERT(expression)                                \
  ((expression) ? (void)0                                    \
                : u7_testing_assert_fail(__FILE__, __LINE__, \
                                         "assertion failed: %s", #expression))

// Asserts actual_ == expected_. Any two integer types (signed or unsigned,
// any width) and floating-point types may be mixed freely and are compared
// without the sign/precision surprises of C's implicit conversions. A
// char*/const char* is always compared as a NUL-terminated C string, NULL
// included; for pointer identity, compare the pointers directly instead.
//
// Comparisons that share no common type (e.g. a number against a string) are
// rejected by the compiler under -Werror (this project always builds with
// it); without -Werror it is merely a warning, and the mismatch is instead
// caught at runtime with a diagnostic. Notably, comparing a pointer against
// a literal 0/NULL is not supported; use U7_ASSERT(ptr == NULL) for that.
#define U7_ASSERT_EQ(actual_, expected_)                                    \
  do {                                                                      \
    (void)sizeof(1 ? (actual_) : (expected_)); /* comparable at all? */     \
    u7_testing_assert_eq(U7_TESTING_VALUE(actual_),                         \
                         U7_TESTING_VALUE(expected_), #actual_, #expected_, \
                         __FILE__, __LINE__);                               \
  } while (0)

#define U7_ASSERT_ERROR_CODE(error_, expected_) \
  U7_ASSERT_EQ((error_).error_code, (expected_))

// Asserts that error_ is ok (error_code == 0). On failure, reports its
// category, code, and message via U7_ERROR_FMT, unlike a plain
// U7_ASSERT_ERROR_CODE(error_, 0) which would only show the code. Requires
// "@/public/error.h" to already be included at the point of use, same as
// U7_ASSERT_ERROR_CODE.
#define U7_ASSERT_OK(error_)                                                 \
  do {                                                                       \
    const u7_error u7_testing_ok_ = (error_);                                \
    if (u7_testing_ok_.error_code != 0) {                                    \
      u7_testing_assert_fail(__FILE__, __LINE__,                             \
                             "expected %s to be ok; actual: %" U7_ERROR_FMT, \
                             #error_, U7_ERROR_FMT_PARAMS(u7_testing_ok_));  \
    }                                                                        \
  } while (0)

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // U7_TESTING_H_
