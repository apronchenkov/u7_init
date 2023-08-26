## A library for error handling in C language

The key properties are:

1. You can pre-allocate errors like "out-of-memory".

2. A simple error forwarding pattern based on a macro:
```c
    U7_RETURN_IF_ERROR(fn());
```

Or the same achieved manually:
```c
    u7_error error = u7_ok();
    if ((error = fn()).error_code) {
      return err;
    }
    return u7_ok();
```

3. Error handling pattern:
```c
    u7_error error = u7_ok();
    if ((error = fn()).error_code) {
      fprintf(stderr, "%" U7_ERROR_FMT "\n", U7_ERROR_FMT_PARAMS(error));
      <error-handling>
      u7_error_release(error);
    }
```


## An initialization library for C language

The library provides a mechanism for initializers that can be registered in any module and triggered from `main()`.

Registration:
```c
  // any.c
  #include <github.com/apronchenkov/u7_init/public/error.h>
  #include <github.com/apronchenkov/u7_init/public/init.h>

  static u7_error initializer_fn(void) {
    // initialization logic
    return u7_ok();
  }

  U7_REGISTER_INITIALIZER((int) precedence, (const char*) name, &initializer_fn)
```

Triggering:
```c
  // main.c
  #include <github.com/apronchenkov/u7_init/public/error.h>
  #include <github.com/apronchenkov/u7_init/public/init.h>
  #include <stdio.h>

  int main(int argc, char** argv) {
    u7_error error = u7_init();
    if (error.error_code) {
      fprintf(stderr, "%" U7_ERROR_FMT "\n", U7_ERROR_FMT_PARAMS(error));
      u7_error_release(error);
      return -1;
    }
    ...
  }
```


## A reference counting library
