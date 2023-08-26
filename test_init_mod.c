#include "@/public/init.h"

#include <github.com/apronchenkov/u7_init/public/error.h>
#include <stdio.h>

static u7_error init_foo(void) {
  puts("foo");
  return u7_ok();
}

static u7_error init_bar(void) {
  puts("bar");
  return u7_ok();
}

static u7_error init_buz(void) {
  puts("buz");
  return u7_ok();
}

U7_REGISTER_INITIALIZER(0, "foo", &init_foo)

U7_REGISTER_INITIALIZER(0, "bar", &init_bar)

U7_REGISTER_INITIALIZER(1, "buz", &init_buz)
