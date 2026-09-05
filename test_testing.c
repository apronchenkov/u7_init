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
  char* argv[] = {"prog"};
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
  char* argv[] = {"prog", "--filter", "case_a"};
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
  char* argv[] = {"prog", "--filter=case_a"};
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
  char* argv[] = {"prog", "--list"};
  int result = u7_testing_run(2, argv, cases, U7_ARRAY_SIZE(cases));
  U7_ASSERT_EQ(result, 0);
  U7_ASSERT_EQ(a_calls, 0);
  U7_ASSERT_EQ(b_calls, 0);
}

U7_TEST(test_run_rejects_unknown_argument) {
  struct u7_testing_case cases[] = {
      U7_TESTING_CASE(case_a),
  };
  char* argv[] = {"prog", "--bogus"};
  int result = u7_testing_run(2, argv, cases, U7_ARRAY_SIZE(cases));
  U7_ASSERT_EQ(result, 2);
}

U7_TEST(test_run_reports_no_match_for_filter) {
  struct u7_testing_case cases[] = {
      U7_TESTING_CASE(case_a),
  };
  char* argv[] = {"prog", "--filter", "nonexistent"};
  int result = u7_testing_run(3, argv, cases, U7_ARRAY_SIZE(cases));
  U7_ASSERT_EQ(result, 2);
}

int main(int argc, char** argv) {
  return u7_testing_run_registered(argc, argv);
}
