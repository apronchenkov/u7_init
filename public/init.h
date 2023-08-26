#ifndef U7_INIT_H_
#define U7_INIT_H_

#include "@/public/error.h"
#include "@/public/refcount.h"

#ifdef __cplusplus
extern "C" {
#endif

// Executes the registered initializers.
//
// Initialisers are executed in order of precedence, with the name as
// the tie-breaker. Lower precedence and lexicographically earlier names are
// executed first.
//
// All initializer names must be unique.
__attribute__((__warn_unused_result__)) u7_error u7_init(void);

// A macro to register an initialiser.
//
// You should avoid registering initialisers from the headers, preferring
// registration in c/cc/cpp files because registration within a header may lead
// to multiple registrations of the same initialiser (that is unlikely what you
// want), and cause name collisions.
//
// Args:
//   precedence: (int) A precedence; a lower value implies earlier execution.
//   name: (str) A null-terminated name; all initialiser names must be unique.
//   init_fn: (u7_error (*)(void)) A pointer to a static function.
//
#define U7_REGISTER_INITIALIZER(precedence, name, init_fn) \
  U7_REGISTER_INITIALIZER_IMPL(__COUNTER__, precedence, name, init_fn)

//
// Private API.
//

#define U7_REGISTER_INITIALIZER_IMPL(counter, precedence_, name_, init_fn_) \
  __attribute__((constructor)) static void U7_REGISTER_INITIALIZER_CONCAT(  \
      u7_init_record_ctor_, counter)(void) {                                \
    static struct u7_initializer record = {                                 \
        .precedence = (precedence_),                                        \
        .name = (name_),                                                    \
        .init_fn = (init_fn_),                                              \
    };                                                                      \
    u7_initializer_register(&record);                                       \
  }

#define U7_REGISTER_INITIALIZER_CONCAT_IMPL(x, y) x##y
#define U7_REGISTER_INITIALIZER_CONCAT(x, y) \
  U7_REGISTER_INITIALIZER_CONCAT_IMPL(x, y)

struct u7_initializer {
  int precedence;
  char const* name;
  u7_error (*init_fn)(void);
  struct u7_initializer* next;
};

void u7_initializer_register(struct u7_initializer* record);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_INIT_H_
