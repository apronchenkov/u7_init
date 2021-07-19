#ifndef U7_MEMORY_ALLOCATOR_H_
#define U7_MEMORY_ALLOCATOR_H_

#include <github.com/apronchenkov/error/public/error.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// Memory allocator interface.
//
// This interface provides no standard way to copy, move, own or destroy
// an allocator.
//
// The expected pattern is that a subsystem receives a memory allocator from
// external and its lifetime exceeds the lifetime of the subsystem.
//
// NOTE: If the subsystem is a singleton, like in the case with u7_error, the
// allocator has to be set globally.
//
struct u7_memory_allocator;

// Allocates memory using the allocator.
typedef u7_error (*u7_memory_allocator_alloc_fn_t)(
    struct u7_memory_allocator* self, size_t size, void** result);

// Deallocates memory, previously allocated using the allocator.
typedef void (*u7_memory_allocator_free_fn_t)(struct u7_memory_allocator* self,
                                              void* ptr);

struct u7_memory_allocator {
  int alignment;  // read-only
  u7_memory_allocator_alloc_fn_t alloc_fn;
  u7_memory_allocator_free_fn_t free_fn;
};

static inline u7_error u7_memory_allocator_alloc(
    struct u7_memory_allocator* self, size_t size, void** result) {
  return self->alloc_fn(self, size, result);
}

static inline void u7_memory_allocator_free(struct u7_memory_allocator* self,
                                            void* ptr) {
  self->free_fn(self, ptr);
}

struct u7_memory_allocator* u7_default_memory_allocator();

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // U7_MEMORY_ALLOCATOR_H_
