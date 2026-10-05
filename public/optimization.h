#ifndef U7_OPTIMIZATION_H_
#define U7_OPTIMIZATION_H_

#ifdef __has_builtin
#define U7_HAS_BUILTIN(builtin) __has_builtin(builtin)
#else
#define U7_HAS_BUILTIN(builtin) (0)
#endif

#if U7_HAS_BUILTIN(__builtin_expect)

// Hints that expr is likely to be true.
#define U7_LIKELY(expr) __builtin_expect(!!(expr), 1)

// Hints that expr is likely to be false.
#define U7_UNLIKELY(expr) __builtin_expect(!!(expr), 0)

#else
#define U7_LIKELY(expr) (!!(expr))
#define U7_UNLIKELY(expr) (!!(expr))
#endif

// Assumes cond is true for optimisation; cond must have no side effects.
#if U7_HAS_BUILTIN(__builtin_assume)
#define U7_ASSUME(cond) __builtin_assume(cond)
#elif U7_HAS_BUILTIN(__builtin_unreachable)
#define U7_ASSUME(cond)          \
  do {                          \
    if (!(cond)) {              \
      __builtin_unreachable();  \
    }                           \
  } while (0)
#else
#define U7_ASSUME(cond) ((void)sizeof(cond))
#endif

#endif  // U7_OPTIMIZATION_H_
