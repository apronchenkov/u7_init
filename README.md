## A library for error handling in C language


Currently, the key ideas are:

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
    u7_error error = NULL;
    if ((error = fn()).error_code) {
      fprintf(stderr, "%" U7_ERROR_FMT "\n", U7_ERROR_FMT_PARAMS(error));
      <error-handling>
      u7_error_release(error);
    }
```
