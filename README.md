## A library for error handling in C language


Currently the key ideas are:

1. You can pre-allocate errors like "out-of-memory".

2. A simple error forwarding pattern, like:
```
    if (u7_error* err = fn(arg)) {
      return err;
    }
```
or using a predefined macro:
```
    U7_RETURN_IF_ERROR(fn(arg));
```

3. Error handling pattern:
```
    if (u7_error* err = function()) {
        <error-handling>
        u7_error_release(err);
    }
```
