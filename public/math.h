#ifndef U7_MATH_H_
#define U7_MATH_H_

#include <stdbool.h>
#include <stdint.h>

// Checked add/subtract/multiply helpers.
//
// Each macro returns the result converted to its named destination type and
// sets *overflow if the mathematical result is not representable in that
// type. *overflow is sticky (set via |=): successful operations do not
// clear it, so initialize it to false before starting a chain of
// operations, and check it once at the end.
//
// lhs, rhs, and overflow are each evaluated exactly once.
//
// Operands must be one of: char, signed char, short, int, long, long long,
// unsigned char, unsigned short, unsigned int, unsigned long, unsigned long
// long -- the same list the C side's _Generic dispatch has cases for. bool
// is not in this list and is rejected on both sides.

#ifndef __cplusplus

// C dispatches each operand through a lossless int64_t/uint64_t
// representation (nested _Generic on lhs then rhs), leaving four
// signedness combinations per operation. long/long long are assumed to
// fit in int64_t/uint64_t.

_Static_assert(sizeof(long) <= sizeof(int64_t),
               "long must fit in int64_t losslessly");
_Static_assert(sizeof(long long) <= sizeof(int64_t),
               "long long must fit in int64_t losslessly");
_Static_assert(sizeof(unsigned long) <= sizeof(uint64_t),
               "unsigned long must fit in uint64_t losslessly");
_Static_assert(sizeof(unsigned long long) <= sizeof(uint64_t),
               "unsigned long long must fit in uint64_t losslessly");

#define U7_OVERFLOW_DEFINE_(name, builtin, lhs_t, rhs_t, dest_t)    \
  static inline dest_t name(lhs_t lhs, rhs_t rhs, bool* overflow) { \
    dest_t result;                                                  \
    *overflow |= builtin(lhs, rhs, &result);                        \
    return result;                                                  \
  }

#define U7_OVERFLOW_DEFINE_KIND_PAIRS_(op, builtin, tag, dest_t)       \
  U7_OVERFLOW_DEFINE_(u7_##op##_overflow_i64_i64_to_##tag##_, builtin, \
                      int64_t, int64_t, dest_t)                        \
  U7_OVERFLOW_DEFINE_(u7_##op##_overflow_i64_u64_to_##tag##_, builtin, \
                      int64_t, uint64_t, dest_t)                       \
  U7_OVERFLOW_DEFINE_(u7_##op##_overflow_u64_i64_to_##tag##_, builtin, \
                      uint64_t, int64_t, dest_t)                       \
  U7_OVERFLOW_DEFINE_(u7_##op##_overflow_u64_u64_to_##tag##_, builtin, \
                      uint64_t, uint64_t, dest_t)

U7_OVERFLOW_DEFINE_KIND_PAIRS_(add, __builtin_add_overflow, i32, int32_t)
U7_OVERFLOW_DEFINE_KIND_PAIRS_(add, __builtin_add_overflow, u32, uint32_t)
U7_OVERFLOW_DEFINE_KIND_PAIRS_(add, __builtin_add_overflow, i64, int64_t)
U7_OVERFLOW_DEFINE_KIND_PAIRS_(add, __builtin_add_overflow, u64, uint64_t)

U7_OVERFLOW_DEFINE_KIND_PAIRS_(sub, __builtin_sub_overflow, i32, int32_t)
U7_OVERFLOW_DEFINE_KIND_PAIRS_(sub, __builtin_sub_overflow, u32, uint32_t)
U7_OVERFLOW_DEFINE_KIND_PAIRS_(sub, __builtin_sub_overflow, i64, int64_t)
U7_OVERFLOW_DEFINE_KIND_PAIRS_(sub, __builtin_sub_overflow, u64, uint64_t)

U7_OVERFLOW_DEFINE_KIND_PAIRS_(mul, __builtin_mul_overflow, i32, int32_t)
U7_OVERFLOW_DEFINE_KIND_PAIRS_(mul, __builtin_mul_overflow, u32, uint32_t)
U7_OVERFLOW_DEFINE_KIND_PAIRS_(mul, __builtin_mul_overflow, i64, int64_t)
U7_OVERFLOW_DEFINE_KIND_PAIRS_(mul, __builtin_mul_overflow, u64, uint64_t)

#undef U7_OVERFLOW_DEFINE_KIND_PAIRS_
#undef U7_OVERFLOW_DEFINE_

// Selects i64_fn or u64_fn based on rhs's type.
#define U7_OVERFLOW_DISPATCH_RHS_(rhs, i64_fn, u64_fn) \
  _Generic((rhs),                                      \
      char: (i64_fn),                                  \
      signed char: (i64_fn),                           \
      short: (i64_fn),                                 \
      int: (i64_fn),                                   \
      long: (i64_fn),                                  \
      long long: (i64_fn),                             \
      unsigned char: (u64_fn),                         \
      unsigned short: (u64_fn),                        \
      unsigned int: (u64_fn),                          \
      unsigned long: (u64_fn),                         \
      unsigned long long: (u64_fn))

// Selects one of ii/iu/ui/uu (i = signed, u = unsigned) based on lhs's and
// rhs's types.
#define U7_OVERFLOW_DISPATCH_(lhs, rhs, ii, iu, ui, uu)         \
  _Generic((lhs),                                               \
      char: U7_OVERFLOW_DISPATCH_RHS_((rhs), ii, iu),           \
      signed char: U7_OVERFLOW_DISPATCH_RHS_((rhs), ii, iu),    \
      short: U7_OVERFLOW_DISPATCH_RHS_((rhs), ii, iu),          \
      int: U7_OVERFLOW_DISPATCH_RHS_((rhs), ii, iu),            \
      long: U7_OVERFLOW_DISPATCH_RHS_((rhs), ii, iu),           \
      long long: U7_OVERFLOW_DISPATCH_RHS_((rhs), ii, iu),      \
      unsigned char: U7_OVERFLOW_DISPATCH_RHS_((rhs), ui, uu),  \
      unsigned short: U7_OVERFLOW_DISPATCH_RHS_((rhs), ui, uu), \
      unsigned int: U7_OVERFLOW_DISPATCH_RHS_((rhs), ui, uu),   \
      unsigned long: U7_OVERFLOW_DISPATCH_RHS_((rhs), ui, uu),  \
      unsigned long long: U7_OVERFLOW_DISPATCH_RHS_((rhs), ui, uu))

#define U7_OVERFLOW_MACRO_(op, dest, lhs, rhs, overflow)                       \
  U7_OVERFLOW_DISPATCH_((lhs), (rhs), u7_##op##_overflow_i64_i64_to_##dest##_, \
                        u7_##op##_overflow_i64_u64_to_##dest##_,               \
                        u7_##op##_overflow_u64_i64_to_##dest##_,               \
                        u7_##op##_overflow_u64_u64_to_##dest##_)((lhs), (rhs), \
                                                                 (overflow))

#define U7_ADD_OVERFLOW_I32(lhs, rhs, overflow) \
  U7_OVERFLOW_MACRO_(add, i32, lhs, rhs, overflow)
#define U7_ADD_OVERFLOW_U32(lhs, rhs, overflow) \
  U7_OVERFLOW_MACRO_(add, u32, lhs, rhs, overflow)
#define U7_ADD_OVERFLOW_I64(lhs, rhs, overflow) \
  U7_OVERFLOW_MACRO_(add, i64, lhs, rhs, overflow)
#define U7_ADD_OVERFLOW_U64(lhs, rhs, overflow) \
  U7_OVERFLOW_MACRO_(add, u64, lhs, rhs, overflow)

#define U7_SUB_OVERFLOW_I32(lhs, rhs, overflow) \
  U7_OVERFLOW_MACRO_(sub, i32, lhs, rhs, overflow)
#define U7_SUB_OVERFLOW_U32(lhs, rhs, overflow) \
  U7_OVERFLOW_MACRO_(sub, u32, lhs, rhs, overflow)
#define U7_SUB_OVERFLOW_I64(lhs, rhs, overflow) \
  U7_OVERFLOW_MACRO_(sub, i64, lhs, rhs, overflow)
#define U7_SUB_OVERFLOW_U64(lhs, rhs, overflow) \
  U7_OVERFLOW_MACRO_(sub, u64, lhs, rhs, overflow)

#define U7_MUL_OVERFLOW_I32(lhs, rhs, overflow) \
  U7_OVERFLOW_MACRO_(mul, i32, lhs, rhs, overflow)
#define U7_MUL_OVERFLOW_U32(lhs, rhs, overflow) \
  U7_OVERFLOW_MACRO_(mul, u32, lhs, rhs, overflow)
#define U7_MUL_OVERFLOW_I64(lhs, rhs, overflow) \
  U7_OVERFLOW_MACRO_(mul, i64, lhs, rhs, overflow)
#define U7_MUL_OVERFLOW_U64(lhs, rhs, overflow) \
  U7_OVERFLOW_MACRO_(mul, u64, lhs, rhs, overflow)

#else  // __cplusplus

// C++ preserves each operand's exact type via a generic lambda, so no
// signedness dispatch table is needed. u7_overflow_is_operand_v tests
// membership in the exact same type list as the C side's _Generic dispatch
// -- plain std::is_integral isn't a safe substitute for that list, since
// it's true for both bool and (at least on some standard library
// implementations) __int128, neither of which the C side accepts.

#include <type_traits>

template <typename T>
constexpr bool u7_overflow_is_operand_v = std::disjunction_v<
    std::is_same<T, char>, std::is_same<T, signed char>, std::is_same<T, short>,
    std::is_same<T, int>, std::is_same<T, long>, std::is_same<T, long long>,
    std::is_same<T, unsigned char>, std::is_same<T, unsigned short>,
    std::is_same<T, unsigned int>, std::is_same<T, unsigned long>,
    std::is_same<T, unsigned long long> >;

#define U7_OVERFLOW_LAMBDA_(builtin, dest_t)                        \
  [](auto lhs_, auto rhs_, bool* overflow_) -> dest_t {             \
    using Lhs = decltype(lhs_);                                     \
    using Rhs = decltype(rhs_);                                     \
    static_assert(u7_overflow_is_operand_v<Lhs>,                    \
                  "lhs must be one of the types math.h documents"); \
    static_assert(u7_overflow_is_operand_v<Rhs>,                    \
                  "rhs must be one of the types math.h documents"); \
    dest_t result;                                                  \
    *overflow_ |= builtin(lhs_, rhs_, &result);                     \
    return result;                                                  \
  }

#define U7_ADD_OVERFLOW_I32(lhs, rhs, overflow)                        \
  (U7_OVERFLOW_LAMBDA_(__builtin_add_overflow, int32_t))((lhs), (rhs), \
                                                         (overflow))
#define U7_ADD_OVERFLOW_U32(lhs, rhs, overflow)                         \
  (U7_OVERFLOW_LAMBDA_(__builtin_add_overflow, uint32_t))((lhs), (rhs), \
                                                          (overflow))
#define U7_ADD_OVERFLOW_I64(lhs, rhs, overflow)                        \
  (U7_OVERFLOW_LAMBDA_(__builtin_add_overflow, int64_t))((lhs), (rhs), \
                                                         (overflow))
#define U7_ADD_OVERFLOW_U64(lhs, rhs, overflow)                         \
  (U7_OVERFLOW_LAMBDA_(__builtin_add_overflow, uint64_t))((lhs), (rhs), \
                                                          (overflow))

#define U7_SUB_OVERFLOW_I32(lhs, rhs, overflow)                        \
  (U7_OVERFLOW_LAMBDA_(__builtin_sub_overflow, int32_t))((lhs), (rhs), \
                                                         (overflow))
#define U7_SUB_OVERFLOW_U32(lhs, rhs, overflow)                         \
  (U7_OVERFLOW_LAMBDA_(__builtin_sub_overflow, uint32_t))((lhs), (rhs), \
                                                          (overflow))
#define U7_SUB_OVERFLOW_I64(lhs, rhs, overflow)                        \
  (U7_OVERFLOW_LAMBDA_(__builtin_sub_overflow, int64_t))((lhs), (rhs), \
                                                         (overflow))
#define U7_SUB_OVERFLOW_U64(lhs, rhs, overflow)                         \
  (U7_OVERFLOW_LAMBDA_(__builtin_sub_overflow, uint64_t))((lhs), (rhs), \
                                                          (overflow))

#define U7_MUL_OVERFLOW_I32(lhs, rhs, overflow)                        \
  (U7_OVERFLOW_LAMBDA_(__builtin_mul_overflow, int32_t))((lhs), (rhs), \
                                                         (overflow))
#define U7_MUL_OVERFLOW_U32(lhs, rhs, overflow)                         \
  (U7_OVERFLOW_LAMBDA_(__builtin_mul_overflow, uint32_t))((lhs), (rhs), \
                                                          (overflow))
#define U7_MUL_OVERFLOW_I64(lhs, rhs, overflow)                        \
  (U7_OVERFLOW_LAMBDA_(__builtin_mul_overflow, int64_t))((lhs), (rhs), \
                                                         (overflow))
#define U7_MUL_OVERFLOW_U64(lhs, rhs, overflow)                         \
  (U7_OVERFLOW_LAMBDA_(__builtin_mul_overflow, uint64_t))((lhs), (rhs), \
                                                          (overflow))

#endif  // __cplusplus

#endif  // U7_MATH_H_
