// Copyright 2026 owent
// Sample code for C++20 coroutine channel/receiver model

#include <libcopp/coroutine/callable_promise.h>
#include <libcopp/coroutine/generator_promise.h>

#include <cstdio>
#include <iostream>
#include <memory>
#include <vector>

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

template <class TReceiver>
static inline void reset_channel_value(TReceiver& receiver) {
  auto ctx = receiver.get_context();
  if (ctx) {
    ctx->reset_value();
  }
}

// ===============================================
// Example 1: Basic Channel Usage with C++20 Coroutine
// ===============================================

copp::callable_future<int> basic_consumer(copp::generator_channel_receiver<int> receiver) {
  std::cout << "[Consumer] Waiting for first value..." << std::endl;

  // Use co_await to receive value from channel
  int value1 = co_await receiver;
  std::cout << "[Consumer] Received first value: " << value1 << std::endl;

  // Must reset to receive next value
  reset_channel_value(receiver);

  std::cout << "[Consumer] Waiting for second value..." << std::endl;
  int value2 = co_await receiver;
  std::cout << "[Consumer] Received second value: " << value2 << std::endl;

  reset_channel_value(receiver);

  std::cout << "[Consumer] Waiting for third value..." << std::endl;
  int value3 = co_await receiver;
  std::cout << "[Consumer] Received third value: " << value3 << std::endl;

  co_return value1 + value2 + value3;
}

void example_basic_channel() {
  std::cout << "\n========== Example 1: Basic C++20 Channel Usage ==========\n" << std::endl;

  // Create a channel for int communication
  auto [receiver, sender] = copp::make_channel<int>();

  // Start consumer coroutine
  std::cout << "Starting consumer coroutine..." << std::endl;
  auto consumer_future = basic_consumer(std::move(receiver));

  std::cout << "\nConsumer is now waiting for values..." << std::endl;
  std::cout << "Is consumer ready? " << (consumer_future.is_ready() ? "Yes" : "No") << std::endl;

  // Send values through sender
  std::cout << "\nSending first value: 10" << std::endl;
  sender->set_value(10);

  std::cout << "\nSending second value: 20" << std::endl;
  sender->set_value(20);

  std::cout << "\nSending third value: 30" << std::endl;
  sender->set_value(30);

  std::cout << "\nConsumer completed with sum: " << consumer_future.get_internal_promise().data() << std::endl;
}

// ===============================================
// Example 2: Producer/Consumer Pattern
// ===============================================

copp::callable_future<void> producer_coroutine(copp::generator_channel_sender<int> sender, int count) {
  std::cout << "[Producer] Starting to produce " << count << " values..." << std::endl;

  for (int i = 1; i <= count; ++i) {
    int value = i * 100;
    std::cout << "[Producer] Producing value " << i << ": " << value << std::endl;

    // Send value to channel
    sender->set_value(value);

    // Yield to let consumer process
    auto current_status = co_yield copp::callable_future<void>::yield_status();
    (void)current_status;
  }

  std::cout << "[Producer] Production complete!" << std::endl;
  co_return;
}

copp::callable_future<int> consumer_coroutine(copp::generator_channel_receiver<int> receiver, int count) {
  std::cout << "[Consumer] Starting to consume " << count << " values..." << std::endl;

  int sum = 0;
  for (int i = 1; i <= count; ++i) {
    std::cout << "[Consumer] Waiting for value " << i << "..." << std::endl;

    int value = co_await receiver;
    std::cout << "[Consumer] Received value " << i << ": " << value << std::endl;

    sum += value;
    reset_channel_value(receiver);
  }

  std::cout << "[Consumer] Consumption complete! Total sum: " << sum << std::endl;
  co_return sum;
}

void example_producer_consumer() {
  std::cout << "\n========== Example 2: Producer/Consumer Pattern ==========\n" << std::endl;

  auto [receiver, sender] = copp::make_channel<int>();

  // Start both producer and consumer
  auto consumer = consumer_coroutine(std::move(receiver), 5);
  auto producer = producer_coroutine(sender, 5);

  std::cout << "\nBoth coroutines completed!" << std::endl;
  std::cout << "Final sum: " << consumer.get_internal_promise().data() << std::endl;
}

// ===============================================
// Example 3: Error Handling
// ===============================================

struct custom_error_transform {
  int operator()(copp::promise_status status) const {
    if (status == copp::promise_status::kKilled) {
      std::cout << "[Error Transform] Coroutine was killed, returning -999" << std::endl;
      return -999;
    } else if (status == copp::promise_status::kTimeout) {
      std::cout << "[Error Transform] Timeout occurred, returning -888" << std::endl;
      return -888;
    }
    std::cout << "[Error Transform] Other error: " << static_cast<int>(status) << ", returning -1" << std::endl;
    return -1;
  }
};

copp::callable_future<int> consumer_with_error_handling(
    copp::generator_channel_receiver<int, custom_error_transform> receiver) {
  std::cout << "[Consumer] Waiting for value (with custom error handling)..." << std::endl;

  // This will use custom_error_transform if an error occurs
  int value = co_await receiver;

  std::cout << "[Consumer] Received value or error default: " << value << std::endl;
  co_return value;
}

void example_error_handling() {
  std::cout << "\n========== Example 3: Error Handling with Custom Transform ==========\n" << std::endl;

  auto [receiver, sender] = copp::make_channel<int, custom_error_transform>();

  auto consumer = consumer_with_error_handling(std::move(receiver));

  std::cout << "\nKilling consumer to demonstrate error handling..." << std::endl;
  consumer.kill(copp::promise_status::kKilled, true);

  std::cout << "Consumer result after kill: " << consumer.get_internal_promise().data() << std::endl;
}

// ===============================================
// Example 4: Void Channel
// ===============================================

copp::callable_future<void> void_channel_consumer(copp::generator_channel_receiver<void> receiver) {
  std::cout << "[Consumer] Waiting for signal 1..." << std::endl;
  co_await receiver;
  std::cout << "[Consumer] Received signal 1!" << std::endl;

  reset_channel_value(receiver);

  std::cout << "[Consumer] Waiting for signal 2..." << std::endl;
  co_await receiver;
  std::cout << "[Consumer] Received signal 2!" << std::endl;

  reset_channel_value(receiver);

  std::cout << "[Consumer] Waiting for signal 3..." << std::endl;
  co_await receiver;
  std::cout << "[Consumer] Received signal 3!" << std::endl;

  co_return;
}

void example_void_channel() {
  std::cout << "\n========== Example 4: Void Channel (Signaling) ==========\n" << std::endl;

  auto [receiver, sender] = copp::make_channel<void>();

  auto consumer = void_channel_consumer(std::move(receiver));

  std::cout << "\nSending signal 1..." << std::endl;
  sender->set_value();

  std::cout << "\nSending signal 2..." << std::endl;
  sender->set_value();

  std::cout << "\nSending signal 3..." << std::endl;
  sender->set_value();

  std::cout << "\nConsumer completed!" << std::endl;
}

// ===============================================
// Example 5: Comparison of Generator Future Types
// ===============================================

void example_vtable_types() {
  std::cout << "\n========== Example 5: Generator VTable Types ==========\n" << std::endl;

  std::cout << "libcopp provides three generator vtable types:\n" << std::endl;

  std::cout << "1. generator_vtable_type::kDefault" << std::endl;
  std::cout << "   - Uses std::function for callbacks (most flexible)" << std::endl;
  std::cout << "   - Can capture complex state in lambdas" << std::endl;
  std::cout << "   - Type: copp::generator_future<T>" << std::endl;
  std::cout << "   - Best for: Complex scenarios with stateful callbacks\n" << std::endl;

  std::cout << "2. generator_vtable_type::kLightWeight" << std::endl;
  std::cout << "   - Uses function pointers instead of std::function" << std::endl;
  std::cout << "   - Lower overhead, faster performance" << std::endl;
  std::cout << "   - Type: copp::generator_lightweight_future<T>" << std::endl;
  std::cout << "   - Best for: Performance-critical code with simple callbacks\n" << std::endl;

  std::cout << "3. generator_vtable_type::kNone" << std::endl;
  std::cout << "   - No callbacks, designed for channel/receiver pattern" << std::endl;
  std::cout << "   - Minimal overhead, most efficient" << std::endl;
  std::cout << "   - Type: copp::generator_channel_future<T> (used by make_channel)" << std::endl;
  std::cout << "   - Best for: Channel-based communication between coroutines\n" << std::endl;

  std::cout << "Recommendation:" << std::endl;
  std::cout << "  - For channels: Use make_channel() (kNone vtable)" << std::endl;
  std::cout << "  - For simple futures: Use generator_lightweight_future (kLightWeight)" << std::endl;
  std::cout << "  - For complex futures: Use generator_future (kDefault)\n" << std::endl;
}

// ===============================================
// Main
// ===============================================

int main() {
  example_basic_channel();
  example_producer_consumer();
  example_error_handling();
  example_void_channel();
  example_vtable_types();

  std::cout << "\n========== All C++20 Channel Examples Completed! ==========\n" << std::endl;
  return 0;
}

#else

int main() {
  std::cerr << "C++20 coroutine is not enabled, sample is disabled" << std::endl;
  return 0;
}

#endif
