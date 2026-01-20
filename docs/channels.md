# Channel/Receiver Pattern

Channels provide a powerful communication mechanism between coroutines in libcopp. They enable producer-consumer patterns and synchronization between both stackful and stackless (C++20) coroutines.

## Overview

The channel/receiver pattern in libcopp allows coroutines to send and receive values asynchronously:

- **Sender**: Produces values and sends them through a channel
- **Receiver**: Waits for and receives values from a channel
- **Channel**: The communication medium connecting senders and receivers

Channels support:

- ✅ **Producer/Consumer patterns**: One-to-one communication
- ✅ **Value transmission**: Send any copyable/movable type
- ✅ **Void channels**: Signal-only communication (no value)
- ✅ **Error handling**: Custom error transformations
- ✅ **Both coroutine types**: Stackful and stackless coroutines

**Note**: Channels do NOT support true broadcasting/multicasting. While multiple coroutines can wait on the same receiver, they share the underlying context. When one receiver calls `reset_value()`, it affects all receivers sharing that context.

---

## Stackful Coroutine Channels

Stackful channels work with `cotask::task` and `copp::coroutine_context`.

### API Reference

#### Creating a Channel

```cpp
#include <libcopp/coroutine/stackful_channel.h>

auto [receiver, sender] = copp::make_stackful_channel<int>();
```

**Returns**: A pair of `stackful_channel_receiver<T>` and `stackful_channel_sender<T>`

#### `stackful_channel_receiver<T>`

The receiver waits for values from the channel.

**Methods**:

- `void reset_value()`: Reset the receiver to receive the next value
- `bool is_ready() const`: Check if a value is ready
- `bool is_pending() const`: Check if waiting for a value

**Usage with `cotask::task::await_value()`**:

```cpp
class consumer_action : public cotask::impl::task_action_impl {
  copp::stackful_channel_receiver<int> receiver_;
  
  int operator()(void*) override {
    // Wait for and receive a value
    int value = cotask::task<>::this_task()->await_value(receiver_);
    
    // Reset for next value
    receiver_.reset_value();
    
    return 0;
  }
};
```

**With error transform**:

```cpp
auto error_transform = [](copp::copp_error_code err) {
  std::cerr << "Error: " << static_cast<int>(err) << std::endl;
  return -1;  // Default value on error
};

int value = cotask::task<>::this_task()->await_value(receiver_, error_transform);
```

**Usage with `inject_await()` for low-level control**:

```cpp
int consumer_runner(void* data_ptr) {
  auto* receiver = static_cast<copp::stackful_channel_receiver<int>*>(data_ptr);
  
  // Get current coroutine context
  auto* ctx = copp::this_coroutine::get<copp::coroutine_context>();
  
  // Directly await on receiver
  auto error_transform = [](copp::copp_error_code err) { return 0; };
  int value = receiver->inject_await(ctx, error_transform);
  
  receiver->reset_value();
  return 0;
}
```

#### `stackful_channel_sender<T>`

The sender sends values through the channel.

**Methods**:

- `void set_value(T&& value)`: Send a value through the channel
- `void set_value(const T& value)`: Send a value through the channel

**Usage**:

```cpp
copp::stackful_channel_sender<int> sender = /* ... */;

// Send a value
sender.set_value(42);

// All waiting receivers will be resumed
```

### Complete Example

```cpp
#include <libcopp/coroutine/stackful_channel.h>
#include <libcotask/task.h>

class producer_action : public cotask::impl::task_action_impl {
  copp::stackful_channel_sender<int> sender_;
  
  int operator()(void*) override {
    for (int i = 1; i <= 5; ++i) {
      sender_.set_value(i * 100);
    }
    return 0;
  }
};

class consumer_action : public cotask::impl::task_action_impl {
  copp::stackful_channel_receiver<int> receiver_;
  
  int operator()(void*) override {
    for (int i = 1; i <= 5; ++i) {
      int value = cotask::task<>::this_task()->await_value(receiver_);
      std::cout << "Received: " << value << std::endl;
      receiver_.reset_value();
    }
    return 0;
  }
};

int main() {
  auto [receiver, sender] = copp::make_stackful_channel<int>();
  
  auto consumer = cotask::task<>::create(consumer_action(std::move(receiver)));
  auto producer = cotask::task<>::create(producer_action(std::move(sender)));
  
  consumer->start();
  producer->start();
  
  return 0;
}
```

**See**: [sample/sample_readme_13.cpp](https://github.com/owent/libcopp/blob/v2/sample/sample_readme_13.cpp) for full examples including:
- Basic producer/consumer
- Error handling with custom transforms
- Direct usage with `copp::coroutine_context`

---

## C++20 Coroutine Channels

Stackless channels work with C++20 coroutines (`copp::callable_future`, `copp::generator_future`).

### API Reference

#### Creating a Channel

```cpp
#include <libcopp/coroutine/generator_promise.h>

auto [receiver, sender] = copp::make_channel<int>();
```

**Returns**: A pair of `generator_channel_receiver<T>` and `generator_channel_sender<T>`

**Types**:
- `generator_channel_receiver<T, ErrorTransform>`: Alias for `generator_channel_future<T, ErrorTransform>`
- `generator_channel_sender<T, ErrorTransform>`: A smart pointer (`context_pointer_type`) to the channel context

#### `generator_channel_receiver<T, ErrorTransform>`

The receiver is an awaitable that suspends the coroutine until a value is available.

**Template Parameters**:
- `T`: Value type (use `void` for signal-only channels)
- `ErrorTransform`: Error transform functor (default: `promise_error_transform<T>`)

**Methods**:
- `void reset_value()`: Reset the receiver to receive the next value
- `bool is_ready() const`: Check if a value is ready
- `bool is_pending() const`: Check if waiting for a value
- `promise_status get_status() const`: Get current status

**Usage with `co_await`**:

```cpp
copp::callable_future<int> consumer(copp::generator_channel_receiver<int> receiver) {
  // Suspend until value is available
  int value = co_await receiver;
  
  std::cout << "Received: " << value << std::endl;
  
  // Reset for next value
  receiver.reset_value();
  
  co_return value;
}
```

**For `void` channels**:

```cpp
copp::callable_future<void> consumer(copp::generator_channel_receiver<void> receiver) {
  // Wait for signal
  co_await receiver;
  std::cout << "Signal received!" << std::endl;
  
  receiver.reset_value();
  co_return;
}
```

#### `generator_channel_sender<T, ErrorTransform>`

The sender is a smart pointer to the channel context.

**Methods**:
- `void set_value(T&& value)`: Send a value (for non-void channels)
- `void set_value(const T& value)`: Send a value (for non-void channels)
- `void set_value()`: Send a signal (for void channels)

**Usage**:

```cpp
copp::generator_channel_sender<int> sender = /* ... */;

// Send a value
sender->set_value(42);  // Note: use -> operator

// All waiting coroutines will be resumed
```

#### Custom Error Transform

Define custom error handling for channel operations:

```cpp
struct custom_error_transform {
  int operator()(copp::promise_status status) const {
    if (status == copp::promise_status::kKilled) {
      return -999;
    } else if (status == copp::promise_status::kTimeout) {
      return -888;
    }
    return -1;
  }
};

auto [receiver, sender] = copp::make_channel<int, custom_error_transform>();

auto consumer_future = consumer(std::move(receiver));

// Kill the consumer to trigger error transform
consumer_future.kill(copp::promise_status::kKilled, true);

// consumer_future.get_internal_promise().data() == -999
```

### Complete Example

```cpp
#include <libcopp/coroutine/callable_promise.h>
#include <libcopp/coroutine/generator_promise.h>

copp::callable_future<void> producer(copp::generator_channel_sender<int> sender, int count) {
  for (int i = 1; i <= count; ++i) {
    sender->set_value(i * 100);
    co_await copp::callable_future<void>::yield();
  }
  co_return;
}

copp::callable_future<int> consumer(copp::generator_channel_receiver<int> receiver, int count) {
  int sum = 0;
  for (int i = 1; i <= count; ++i) {
    int value = co_await receiver;
    sum += value;
    receiver.reset_value();
  }
  co_return sum;
}

int main() {
  auto [receiver, sender] = copp::make_channel<int>();
  
  auto consumer_future = consumer(std::move(receiver), 5);
  auto producer_future = producer(sender, 5);
  
  int total = consumer_future.get_internal_promise().data();
  // total == 1500 (100 + 200 + 300 + 400 + 500)
  
  return 0;
}
```

**See**: [sample/sample_readme_14.cpp](https://github.com/owent/libcopp/blob/v2/sample/sample_readme_14.cpp) for full examples including:
- Basic channel usage
- Producer/consumer pattern
- Error handling with custom transforms
- Void channels for signaling
- Comparison of generator vtable types

---

## Usage Patterns

### Producer/Consumer

One producer sends values, one consumer receives them.

**Best for**: Pipeline processing, task queues

```cpp
// Stackful
auto [receiver, sender] = copp::make_stackful_channel<int>();

// C++20
auto [receiver, sender] = copp::make_channel<int>();
```

### Signal-only Communication

Use `void` channels when you only need to signal events without data.

**Best for**: Synchronization, wake-up notifications

```cpp
auto [receiver, sender] = copp::make_channel<void>();

// Sender
sender->set_value();  // Send signal

// Receiver
co_await receiver;  // Wait for signal
```

---

## Generator VTable Types (C++20 Only)

libcopp provides three generator vtable types for different performance/flexibility tradeoffs:

### `generator_vtable_type::kDefault`

**Type**: `copp::generator_future<T>`

- Uses `std::function` for callbacks
- Most flexible - can capture complex state
- Higher overhead due to type erasure
- **Best for**: Complex scenarios with stateful callbacks

```cpp
copp::generator_future<int> future{
  [captured_state](copp::generator_future<int>::context_pointer_type ctx) {
    // Can capture complex state
  },
  [](const copp::generator_future<int>::context_type& ctx) {
    // Resume callback
  }
};
```

### `generator_vtable_type::kLightWeight`

**Type**: `copp::generator_lightweight_future<T>`

- Uses function pointers instead of `std::function`
- Lower overhead, faster performance
- Cannot capture state in callbacks
- **Best for**: Performance-critical code with simple callbacks

```cpp
void on_suspend(copp::generator_lightweight_future<int>::context_pointer_type ctx) {
  // Function pointer - no captures
}

copp::generator_lightweight_future<int> future{on_suspend};
```

### `generator_vtable_type::kNone`

**Type**: `copp::generator_channel_future<T>` (used by `make_channel()`)

- No callbacks - designed for channel pattern
- Minimal overhead, most efficient
- **Best for**: Channel-based communication

```cpp
auto [receiver, sender] = copp::make_channel<int>();
// receiver is generator_channel_future<int>
```

### Recommendation

| Use Case | Recommended Type |
|----------|-----------------|
| Channel communication | `make_channel()` (kNone) |
| Simple futures | `generator_lightweight_future` (kLightWeight) |
| Complex futures with state | `generator_future` (kDefault) |

---

## Performance Considerations

### When to Use Stackful vs Stackless

| Feature | Stackful Channels | Stackless Channels (C++20) |
|---------|------------------|---------------------------|
| **Stack usage** | Allocates stack per coroutine | No stack allocation |
| **Compatibility** | Works on all C++11+ compilers | Requires C++20 support |
| **Integration** | Works with existing callback APIs | Better with async/await code |
| **Performance** | Slightly higher overhead | Lower overhead |
| **Debugging** | Easier to debug stack traces | Harder to debug state machines |

**Recommendation**:
- Use **stackful** if you need C++11/14/17 compatibility or work with callback-heavy code
- Use **stackless** if you have C++20 and want modern async/await syntax

### Channel Best Practices

1. **Always `reset_value()` after receiving**
   ```cpp
   int value = co_await receiver;
   receiver.reset_value();  // Required for next receive
   ```

2. **Avoid blocking senders**
   - Channels don't buffer values
   - Sender resumes all waiting receivers immediately
   - If no receivers are waiting, the value is stored until next `co_await`

3. **Error handling**
   - Always provide error transforms for production code
   - Handle `promise_status::kKilled` and `kTimeout` cases
   - Default error transform returns default-constructed value

---

## Common Pitfalls

### Forgetting to Reset

```cpp
// ❌ Wrong
int v1 = co_await receiver;
int v2 = co_await receiver;  // Returns same value!

// ✅ Correct
int v1 = co_await receiver;
receiver.reset_value();
int v2 = co_await receiver;
```

### Sender Access Pattern Difference

```cpp
// Stackful
copp::stackful_channel_sender<int> sender;
sender.set_value(42);  // Direct method call

// Stackless (C++20)
copp::generator_channel_sender<int> sender;  // This is a smart pointer
sender->set_value(42);  // Use -> operator
```

### Not Handling Errors

```cpp
// ❌ Without error handling
int value = co_await receiver;  // May return default value on error

// ✅ With error handling
auto [receiver, sender] = copp::make_channel<int, custom_error_transform>();
```

---

## See Also

- [Examples](examples/index.md) - Full working examples
- [sample_readme_13.cpp](https://github.com/owent/libcopp/blob/v2/sample/sample_readme_13.cpp) - Stackful channel examples
- [sample_readme_14.cpp](https://github.com/owent/libcopp/blob/v2/sample/sample_readme_14.cpp) - C++20 channel examples
- [API Reference](doxygen/html/) - Detailed API documentation
