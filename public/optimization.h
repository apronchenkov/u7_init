#ifndef U7_OPTIMIZATION_H_
#define U7_OPTIMIZATION_H_

#ifdef __has_builtin
#define U7_HAS_BUILTIN(builtin) __has_builtin(builtin)
#else
#define U7_HAS_BUILTIN(builtin) (0)
#endif

#if U7_HAS_BUILTIN(__builtin_expect)
#define U7_LIKELY(expr) __builtin_expect(!!(expr), 1)
#define U7_UNLIKELY(expr) __builtin_expect(!!(expr), 0)
#else
#define U7_LIKELY(expr) (!!(expr))
#define U7_UNLIKELY(expr) (!!(expr))
#endif

#endif  // U7_OPTIMIZATION_H_
