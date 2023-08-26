#include "@/public/init.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

static struct u7_initializer* u7_init_record_head = NULL;

static int u7_initizlizer_cmp_by_name(const void* lhs, const void* rhs) {
  return strcmp((**(struct u7_initializer**)lhs).name,
                (**(struct u7_initializer**)rhs).name);
}

static int u7_initizlizer_cmp(const void* lhs, const void* rhs) {
  if ((**(struct u7_initializer**)lhs).precedence <
      (**(struct u7_initializer**)rhs).precedence) {
    return -1;
  } else if ((**(struct u7_initializer**)lhs).precedence >
             (**(struct u7_initializer**)rhs).precedence) {
    return 1;
  }
  return u7_initizlizer_cmp_by_name(lhs, rhs);
}

u7_error u7_init(void) {
  size_t count = 0;
  for (struct u7_initializer* it = u7_init_record_head; it; it = it->next) {
    count += 1;
  }
  struct u7_initializer** array =
      (struct u7_initializer**)malloc(count * sizeof(struct u7_initializer*));
  if (array == NULL) {
    return u7_errnof(ENOMEM, "u7_init: malloc: failed to allocate buffer");
  }
  size_t i = 0;
  for (struct u7_initializer* it = u7_init_record_head; it; it = it->next) {
    array[i++] = it;
  }
  qsort(array, count, sizeof(array[0]), &u7_initizlizer_cmp_by_name);
  for (size_t i = 1; i < count; ++i) {
    if (strcmp(array[i - 1]->name, array[i]->name) == 0) {
      return u7_errnof(EINVAL, "u7_int: non-unique initializer name: %s",
                       array[i]->name);
    }
  }
  qsort(array, count, sizeof(array[0]), &u7_initizlizer_cmp);
  for (size_t i = 0; i < count; ++i) {
    u7_error error = array[i]->init_fn();
    if (error.error_code) {
      free(array);
      return error;
    }
  }
  free(array);
  u7_init_record_head = NULL;
  return u7_ok();
}

void u7_initializer_register(struct u7_initializer* record) {
  record->next = u7_init_record_head;
  u7_init_record_head = record;
}
