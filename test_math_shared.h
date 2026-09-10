// math.h's test bodies, shared between test_math.c and test_math_cpp.cpp so
// the same tests run under both languages without duplicating them.

#ifndef U7_INIT_TEST_MATH_SHARED_H_
#define U7_INIT_TEST_MATH_SHARED_H_

#include "@/public/math.h"
#include "@/public/testing.h"

#include <stdint.h>

U7_TEST(test_i32_detects_overflow) {
  bool overflow = false;
  int32_t result =
      U7_ADD_OVERFLOW_I32((int32_t)INT32_MAX, (int32_t)1, &overflow);
  U7_ASSERT(overflow);
  (void)result;
}

U7_TEST(test_i32_accepts_mixed_width_operands_without_overflow) {
  bool overflow = false;
  uint64_t small_u64 = 5;
  int64_t small_i64 = 10;
  int32_t result = U7_ADD_OVERFLOW_I32(small_u64, small_i64, &overflow);
  U7_ASSERT(!overflow);
  U7_ASSERT_EQ(result, 15);
}

U7_TEST(test_i32_accepts_narrow_operand_types) {
  bool overflow = false;
  int8_t tiny = 3;
  uint16_t small = 4;
  int32_t result = U7_ADD_OVERFLOW_I32(tiny, small, &overflow);
  U7_ASSERT(!overflow);
  U7_ASSERT_EQ(result, 7);
}

U7_TEST(test_i32_detects_overflow_from_a_wider_unsigned_operand) {
  bool overflow = false;
  uint32_t huge = UINT32_MAX;
  int32_t result = U7_ADD_OVERFLOW_I32(huge, (int32_t)0, &overflow);
  U7_ASSERT(overflow);
  (void)result;
}

U7_TEST(test_i32_detects_underflow_from_a_very_negative_operand) {
  bool overflow = false;
  int64_t very_negative = INT64_MIN;
  uint32_t five = 5;
  int32_t result = U7_ADD_OVERFLOW_I32(very_negative, five, &overflow);
  U7_ASSERT(overflow);
  (void)result;
}

U7_TEST(test_u32_negative_lhs_with_non_negative_sum_does_not_overflow) {
  bool overflow = false;
  int32_t neg_one = -1;
  uint32_t five = 5;
  uint32_t result = U7_ADD_OVERFLOW_U32(neg_one, five, &overflow);
  U7_ASSERT(!overflow);
  U7_ASSERT_EQ(result, 4);
}

U7_TEST(test_u32_negative_lhs_with_negative_sum_overflows) {
  bool overflow = false;
  int32_t neg_ten = -10;
  uint32_t five = 5;
  uint32_t result = U7_ADD_OVERFLOW_U32(neg_ten, five, &overflow);
  U7_ASSERT(overflow);
  (void)result;
}

U7_TEST(test_i64_detects_overflow) {
  bool overflow = false;
  int64_t result =
      U7_ADD_OVERFLOW_I64((int64_t)INT64_MAX, (int64_t)1, &overflow);
  U7_ASSERT(overflow);
  (void)result;
}

U7_TEST(test_u64_detects_overflow) {
  bool overflow = false;
  uint64_t result =
      U7_ADD_OVERFLOW_U64((uint64_t)UINT64_MAX, (uint32_t)1, &overflow);
  U7_ASSERT(overflow);
  (void)result;
}

U7_TEST(test_u64_negative_operand_with_non_negative_sum_does_not_overflow) {
  bool overflow = false;
  int64_t neg_three = -3;
  uint64_t result = U7_ADD_OVERFLOW_U64(neg_three, (uint64_t)5, &overflow);
  U7_ASSERT(!overflow);
  U7_ASSERT_EQ(result, 2);
}

U7_TEST(test_overflow_accumulates_across_a_chain) {
  bool overflow = false;
  int32_t acc = U7_ADD_OVERFLOW_I32((int32_t)1, (int32_t)2, &overflow);
  acc = U7_ADD_OVERFLOW_I32(acc, (int32_t)INT32_MAX, &overflow);
  acc = U7_ADD_OVERFLOW_I32(acc, (int32_t)1, &overflow);
  U7_ASSERT(overflow);
  (void)acc;
}

U7_TEST(test_sub_i32_detects_underflow) {
  bool overflow = false;
  int32_t result =
      U7_SUB_OVERFLOW_I32((int32_t)INT32_MIN, (int32_t)1, &overflow);
  U7_ASSERT(overflow);
  (void)result;
}

U7_TEST(test_sub_i32_accepts_mixed_width_operands_without_overflow) {
  bool overflow = false;
  uint16_t ten = 10;
  int8_t three = 3;
  int32_t result = U7_SUB_OVERFLOW_I32(ten, three, &overflow);
  U7_ASSERT(!overflow);
  U7_ASSERT_EQ(result, 7);
}

U7_TEST(test_sub_u32_negative_result_overflows) {
  bool overflow = false;
  uint32_t three = 3;
  int32_t ten = 10;
  uint32_t result = U7_SUB_OVERFLOW_U32(three, ten, &overflow);
  U7_ASSERT(overflow);
  (void)result;
}

U7_TEST(test_sub_u32_non_negative_result_does_not_overflow) {
  bool overflow = false;
  uint32_t ten = 10;
  int32_t three = 3;
  uint32_t result = U7_SUB_OVERFLOW_U32(ten, three, &overflow);
  U7_ASSERT(!overflow);
  U7_ASSERT_EQ(result, 7);
}

U7_TEST(test_sub_i64_detects_underflow) {
  bool overflow = false;
  int64_t result =
      U7_SUB_OVERFLOW_I64((int64_t)INT64_MIN, (int64_t)1, &overflow);
  U7_ASSERT(overflow);
  (void)result;
}

U7_TEST(test_sub_u64_detects_underflow) {
  bool overflow = false;
  uint64_t zero = 0;
  uint64_t one = 1;
  uint64_t result = U7_SUB_OVERFLOW_U64(zero, one, &overflow);
  U7_ASSERT(overflow);
  (void)result;
}

U7_TEST(test_mul_i32_detects_overflow) {
  bool overflow = false;
  int32_t result =
      U7_MUL_OVERFLOW_I32((int32_t)INT32_MAX, (int32_t)2, &overflow);
  U7_ASSERT(overflow);
  (void)result;
}

U7_TEST(test_mul_i32_negating_int_min_overflows) {
  bool overflow = false;
  int32_t result =
      U7_MUL_OVERFLOW_I32((int32_t)-1, (int32_t)INT32_MIN, &overflow);
  U7_ASSERT(overflow);
  (void)result;
}

U7_TEST(test_mul_i32_accepts_mixed_width_operands_without_overflow) {
  bool overflow = false;
  int8_t six = 6;
  uint16_t seven = 7;
  int32_t result = U7_MUL_OVERFLOW_I32(six, seven, &overflow);
  U7_ASSERT(!overflow);
  U7_ASSERT_EQ(result, 42);
}

U7_TEST(test_mul_u32_detects_overflow) {
  bool overflow = false;
  uint32_t result =
      U7_MUL_OVERFLOW_U32((uint32_t)UINT32_MAX, (uint32_t)2, &overflow);
  U7_ASSERT(overflow);
  (void)result;
}

U7_TEST(test_mul_i64_detects_overflow) {
  bool overflow = false;
  int64_t result =
      U7_MUL_OVERFLOW_I64((int64_t)INT64_MAX, (int64_t)2, &overflow);
  U7_ASSERT(overflow);
  (void)result;
}

U7_TEST(test_overflow_accumulates_across_different_operations) {
  bool overflow = false;
  int32_t a = U7_ADD_OVERFLOW_I32((int32_t)1, (int32_t)2, &overflow);
  int32_t b = U7_SUB_OVERFLOW_I32(a, (int32_t)10, &overflow);
  int32_t c = U7_MUL_OVERFLOW_I32(b, (int32_t)INT32_MAX, &overflow);
  U7_ASSERT(overflow);
  (void)c;
}

#endif  // U7_INIT_TEST_MATH_SHARED_H_
