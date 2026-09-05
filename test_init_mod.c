#include "@/public/init.h"

#include <stddef.h>

char u7_testing_initializer_order[4];
static size_t u7_testing_initializer_count;

static void record(char value) {
  u7_testing_initializer_order[u7_testing_initializer_count++] = value;
}

static u7_error init_foo(void) {
  record('f');
  return u7_ok();
}

static u7_error init_bar(void) {
  record('b');
  return u7_ok();
}

static u7_error init_buz(void) {
  record('z');
  return u7_ok();
}

U7_REGISTER_INITIALIZER(0, "foo", &init_foo)

U7_REGISTER_INITIALIZER(0, "bar", &init_bar)

U7_REGISTER_INITIALIZER(1, "buz", &init_buz)
