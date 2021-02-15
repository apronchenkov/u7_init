## A library for error handling in C language


Currently, the key ideas are:

1. You can pre-allocate errors like "out-of-memory".

2. A simple error forwarding pattern based on a macro:
```
    U7_RETURN_IF_ERROR(fn());
```

Or the same achieved manually:
```
    u7_error* error = NULL;
    if ((error = fn())) {
      return err;
    }
```

3. Error handling pattern:
```
    u7_error* error = NULL;
    if ((error = fn())) {
        <error-handling>
        u7_error_release(error);
    }
```
