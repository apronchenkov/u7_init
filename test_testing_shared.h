// testing.h's own test bodies, shared between test_testing.c and
// test_testing_cpp.cpp so the same tests run under both languages without
// duplicating them.

#ifndef U7_INIT_TEST_TESTING_SHARED_H_
#define U7_INIT_TEST_TESTING_SHARED_H_

#include "@/public/testing.h"

static int a_calls;
static int b_calls;

static void case_a(void) { a_calls += 1; }
static void case_b(void) { b_calls += 1; }

U7_TEST(test_run_executes_all_selected_tests) {
  a_calls = 0;
  b_calls = 0;
  struct u7_testing_case cases[] = {
      U7_TESTING_CASE(case_a),
      U7_TESTING_CASE(case_b),
  };
  char prog[] = "prog";
  char* argv[] = {prog};
  int result = u7_testing_run(1, argv, cases, U7_ARRAY_SIZE(cases));
  U7_ASSERT_EQ(result, 0);
  U7_ASSERT_EQ(a_calls, 1);
  U7_ASSERT_EQ(b_calls, 1);
}

U7_TEST(test_run_filter_selects_subset) {
  a_calls = 0;
  b_calls = 0;
  struct u7_testing_case cases[] = {
      U7_TESTING_CASE(case_a),
      U7_TESTING_CASE(case_b),
  };
  char prog[] = "prog";
  char filter_flag[] = "--filter";
  char case_a_name[] = "case_a";
  char* argv[] = {prog, filter_flag, case_a_name};
  int result = u7_testing_run(3, argv, cases, U7_ARRAY_SIZE(cases));
  U7_ASSERT_EQ(result, 0);
  U7_ASSERT_EQ(a_calls, 1);
  U7_ASSERT_EQ(b_calls, 0);
}

U7_TEST(test_run_filter_equals_form_selects_subset) {
  a_calls = 0;
  b_calls = 0;
  struct u7_testing_case cases[] = {
      U7_TESTING_CASE(case_a),
      U7_TESTING_CASE(case_b),
  };
  char prog[] = "prog";
  char filter_eq[] = "--filter=case_a";
  char* argv[] = {prog, filter_eq};
  int result = u7_testing_run(2, argv, cases, U7_ARRAY_SIZE(cases));
  U7_ASSERT_EQ(result, 0);
  U7_ASSERT_EQ(a_calls, 1);
  U7_ASSERT_EQ(b_calls, 0);
}

U7_TEST(test_run_list_does_not_execute_tests) {
  a_calls = 0;
  b_calls = 0;
  struct u7_testing_case cases[] = {
      U7_TESTING_CASE(case_a),
      U7_TESTING_CASE(case_b),
  };
  char prog[] = "prog";
  char list_flag[] = "--list";
  char* argv[] = {prog, list_flag};
  int result = u7_testing_run(2, argv, cases, U7_ARRAY_SIZE(cases));
  U7_ASSERT_EQ(result, 0);
  U7_ASSERT_EQ(a_calls, 0);
  U7_ASSERT_EQ(b_calls, 0);
}

U7_TEST(test_run_rejects_unknown_argument) {
  struct u7_testing_case cases[] = {
      U7_TESTING_CASE(case_a),
  };
  char prog[] = "prog";
  char bogus_flag[] = "--bogus";
  char* argv[] = {prog, bogus_flag};
  int result = u7_testing_run(2, argv, cases, U7_ARRAY_SIZE(cases));
  U7_ASSERT_EQ(result, 2);
}

U7_TEST(test_run_reports_no_match_for_filter) {
  struct u7_testing_case cases[] = {
      U7_TESTING_CASE(case_a),
  };
  char prog[] = "prog";
  char filter_flag[] = "--filter";
  char nonexistent[] = "nonexistent";
  char* argv[] = {prog, filter_flag, nonexistent};
  int result = u7_testing_run(3, argv, cases, U7_ARRAY_SIZE(cases));
  U7_ASSERT_EQ(result, 2);
}

U7_TEST(test_char_equality) {
  char a = 'x';
  char b = 'x';
  U7_ASSERT_EQ(a, b);
}

U7_TEST(test_signed_char_equality) {
  signed char a = -5;
  signed char b = -5;
  U7_ASSERT_EQ(a, b);
}

U7_TEST(test_short_equality) {
  short a = -1234;
  short b = -1234;
  U7_ASSERT_EQ(a, b);
}

U7_TEST(test_int_equality) {
  int a = 42;
  int b = 42;
  U7_ASSERT_EQ(a, b);
}

U7_TEST(test_long_equality) {
  long a = 123456789L;
  long b = 123456789L;
  U7_ASSERT_EQ(a, b);
}

U7_TEST(test_long_long_equality) {
  long long a = 123456789012345LL;
  long long b = 123456789012345LL;
  U7_ASSERT_EQ(a, b);
}

U7_TEST(test_unsigned_char_equality) {
  unsigned char a = 200;
  unsigned char b = 200;
  U7_ASSERT_EQ(a, b);
}

U7_TEST(test_unsigned_short_equality) {
  unsigned short a = 60000;
  unsigned short b = 60000;
  U7_ASSERT_EQ(a, b);
}

U7_TEST(test_unsigned_int_equality) {
  unsigned int a = 4000000000u;
  unsigned int b = 4000000000u;
  U7_ASSERT_EQ(a, b);
}

U7_TEST(test_unsigned_long_equality) {
  unsigned long a = 123456789UL;
  unsigned long b = 123456789UL;
  U7_ASSERT_EQ(a, b);
}

U7_TEST(test_unsigned_long_long_equality) {
  unsigned long long a = 123456789012345ULL;
  unsigned long long b = 123456789012345ULL;
  U7_ASSERT_EQ(a, b);
}

U7_TEST(test_float_equality) {
  float a = 1.5f;
  float b = 1.5f;
  U7_ASSERT_EQ(a, b);
}

U7_TEST(test_double_equality) {
  double a = 2.5;
  double b = 2.5;
  U7_ASSERT_EQ(a, b);
}

U7_TEST(test_char_pointer_equality) {
  char buf1[] = "hello";
  char buf2[] = "hello";
  char* a = buf1;
  char* b = buf2;
  U7_ASSERT_EQ(a, b);
}

U7_TEST(test_const_char_pointer_equality) {
  const char* a = "hello";
  const char* b = "hello";
  U7_ASSERT_EQ(a, b);
}

U7_TEST(test_int_and_unsigned_int_of_equal_value_compare_equal) {
  int a = 5;
  unsigned int b = 5u;
  U7_ASSERT_EQ(a, b);
}

#endif  // U7_INIT_TEST_TESTING_SHARED_H_
