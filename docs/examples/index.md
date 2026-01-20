# Examples

## Coroutine context example

This is a simple example of using basic coroutine context below:

```cpp
{% include "../../sample/sample_readme_1.cpp" %}
```

Also, you can use `copp::coroutine_context_container<ALLOCATOR>` instead of `copp::coroutine_context_default` to use a different stack allocator.

## Coroutine task example

This is a simple example of using coroutine task with lambda expression:

```cpp
{% include "../../sample/sample_readme_2.cpp" %}
```

Also, you can your stack allocator or id allocator by setting different parameters in template class **cotask::task<TCO_MACRO>**.

## Using coroutine task manager

This is a simple example of using task manager:

```cpp
{% include "../../sample/sample_readme_3.cpp" %}
```

## Using stack pool

This is a simple example of using stack pool for cotask:

```cpp
{% include "../../sample/sample_readme_4.cpp" %}
```

## Using then or await_task

This is a simple example of using `then` and `await_task` for cotask:

```cpp
{% include "../../sample/sample_readme_5.cpp" %}
```

## Using c++20 coroutine

```cpp
{% include "../../sample/sample_readme_6.cpp" %}
```

## Using c++20 coroutine with custom generator

```cpp
{% include "../../sample/sample_readme_7.cpp" %}
```

## Custom error (timeout for example) for c++20 coroutine

By implementing `std_coroutine_default_error_transform<CustomType>`, we can transform error code of libcopp to our custom type.

```cpp
{% include "../../sample/sample_readme_8.cpp" %}
```

## Let c++20 coroutine work with cotask::task<MACRO>

This is a simple example to let c++20 coroutine await cotask::task.

```cpp
{% include "../../sample/sample_readme_9.cpp" %}
```

## Using SetUnhandledExceptionFilter on Windows with cotask::task<MACRO>

Some applications will use `SetUnhandledExceptionFilter` to catch unhandled exception and analysis crash problem. But `SetUnhandledExceptionFilter` is only works with [coroutine context of windows fiber](https://github.com/owent/libcopp/blob/v2/include/libcopp/coroutine/coroutine_context_fiber_container.h). This is a sample of using **windows fiber** as coroutine context in `cotask::task<MACRO>`.

```cpp
{% include "../../sample/sample_readme_10.cpp" %}
```

## Using c++20 coroutine with channel receiver and sender

```cpp
{% include "../../sample/sample_readme_11.cpp" %}
```

## Using stackful coroutine task with channel receiver and sender

```cpp
{% include "../../sample/sample_readme_12.cpp" %}
```

## Using stackful coroutine with channel receiver and sender

This example demonstrates the channel/receiver pattern with stackful coroutines, including:

- Basic producer/consumer communication
- Error handling with custom transforms  
- Direct usage with `copp::coroutine_context`

```cpp
{% include "../../sample/sample_readme_13.cpp" %}
```

For detailed documentation on channels, see the [Channels Guide](../channels.md).

## Using C++20 coroutine with channel receiver and sender

This example demonstrates the channel/receiver pattern with C++20 coroutines, including:

- Basic channel usage with `co_await`
- Producer/consumer pattern
- Error handling with custom error transforms
- Void channels for signaling
- Comparison of generator vtable types

```cpp
{% include "../../sample/sample_readme_14.cpp" %}
```

For detailed documentation on channels, see the [Channels Guide](../channels.md).

