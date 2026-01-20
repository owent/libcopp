// Copyright 2025 owent
// Sample code for stackful coroutine channel/receiver model

#include <libcopp/coroutine/coroutine_context_container.h>
#include <libcopp/coroutine/stackful_channel.h>
#include <libcotask/task.h>

#include <cstdio>
#include <iostream>
#include <memory>

// ===============================================
// Example 1: Basic Channel Usage with cotask::task
// ===============================================

class producer_action : public cotask::impl::task_action_impl {
 public:
  producer_action(copp::stackful_channel_sender<int> sender) : sender_(std::move(sender)), value_to_send_(0) {}

  int operator()(void*) override {
    std::cout << "[Producer] Starting production..." << std::endl;

    // Simulate producing values
    for (int i = 1; i <= 5; ++i) {
      value_to_send_ = i * 100;
      std::cout << "[Producer] Producing value: " << value_to_send_ << std::endl;

      // Send value through channel
      sender_.set_value(value_to_send_);

      std::cout << "[Producer] Value sent, yielding..." << std::endl;
    }

    std::cout << "[Producer] Production complete!" << std::endl;
    return 0;
  }

 private:
  copp::stackful_channel_sender<int> sender_;
  int value_to_send_;
};

class consumer_action : public cotask::impl::task_action_impl {
 public:
  consumer_action(copp::stackful_channel_receiver<int> receiver) : receiver_(std::move(receiver)) {}

  int operator()(void*) override {
    std::cout << "[Consumer] Starting consumption..." << std::endl;

    // Receive values from channel
    for (int i = 1; i <= 5; ++i) {
      std::cout << "[Consumer] Waiting for value " << i << "..." << std::endl;

      // await_value will suspend the task until a value is available
      int value = cotask::task<>::this_task()->await_value(receiver_);

      std::cout << "[Consumer] Received value: " << value << std::endl;

      // Reset channel for next value
      receiver_.reset_value();
    }

    std::cout << "[Consumer] Consumption complete!" << std::endl;
    return 0;
  }

 private:
  copp::stackful_channel_receiver<int> receiver_;
};

void example_basic_channel() {
  std::cout << "\n========== Example 1: Basic Channel Usage ==========\n" << std::endl;

  // Create a channel for int communication
  std::pair<copp::stackful_channel_receiver<int>, copp::stackful_channel_sender<int>> channel =
      copp::make_stackful_channel<int>();
  auto receiver = std::move(channel.first);
  auto sender = std::move(channel.second);

  // Create consumer and producer tasks
  auto consumer = cotask::task<>::create(consumer_action(std::move(receiver)));
  auto producer = cotask::task<>::create(producer_action(std::move(sender)));

  // Start consumer (it will suspend immediately waiting for data)
  std::cout << "Starting consumer task..." << std::endl;
  consumer->start();

  std::cout << "\nStarting producer task..." << std::endl;
  producer->start();

  std::cout << "\nBoth tasks completed!" << std::endl;
}

// ===============================================
// Example 2: Error Handling with Custom Transform
// ===============================================

class consumer_with_error_handling : public cotask::impl::task_action_impl {
 public:
  consumer_with_error_handling(copp::stackful_channel_receiver<int> receiver) : receiver_(std::move(receiver)) {}

  int operator()(void*) override {
    std::cout << "[Consumer] Starting with error handling..." << std::endl;

    // First receive with default error handling
    {
      int value = cotask::task<>::this_task()->await_value(receiver_);
      std::cout << "[Consumer] Received: " << value << std::endl;
      receiver_.reset_value();
    }

    // Second receive with custom error transform
    {
      auto error_transform = [](copp::copp_error_code err) {
        std::cout << "[Consumer] Error occurred: " << static_cast<int>(err) << ", returning default value -1"
                  << std::endl;
        return -1;  // Return default value on error
      };

      int value = cotask::task<>::this_task()->await_value(receiver_, error_transform);
      std::cout << "[Consumer] Received or defaulted: " << value << std::endl;
    }

    return 0;
  }

 private:
  copp::stackful_channel_receiver<int> receiver_;
};

void example_error_handling() {
  std::cout << "\n========== Example 2: Error Handling ==========\n" << std::endl;

  std::pair<copp::stackful_channel_receiver<int>, copp::stackful_channel_sender<int>> channel =
      copp::make_stackful_channel<int>();
  auto receiver = std::move(channel.first);
  auto sender = std::move(channel.second);

  auto consumer = cotask::task<>::create(consumer_with_error_handling(std::move(receiver)));

  consumer->start();

  // Send first value normally
  sender.set_value(42);

  // Kill the consumer task to trigger error handling
  std::cout << "\nKilling consumer task to demonstrate error handling..." << std::endl;
  consumer->kill();

  std::cout << "\nTask killed!" << std::endl;
}

// ===============================================
// Example 3: Using with copp::coroutine_context directly
// ===============================================

struct direct_context_example_data {
  copp::stackful_channel_receiver<int> receiver;
  int received_sum = 0;
};

int direct_consumer_runner(void* data_ptr) {
  auto* data = static_cast<direct_context_example_data*>(data_ptr);

  std::cout << "[Direct Consumer] Starting..." << std::endl;

  // Use inject_await with error transform for low-level control
  auto error_transform = [](copp::copp_error_code err) {
    std::cout << "[Direct Consumer] Error: " << static_cast<int>(err) << std::endl;
    return 0;
  };

  for (int i = 0; i < 3; ++i) {
    // Get current coroutine context
    auto* ctx = copp::this_coroutine::get<copp::coroutine_context>();

    std::cout << "[Direct Consumer] Awaiting value " << (i + 1) << "..." << std::endl;

    // Directly use inject_await on receiver
    int value = data->receiver.inject_await(ctx, error_transform);

    std::cout << "[Direct Consumer] Received: " << value << std::endl;
    data->received_sum += value;

    data->receiver.reset_value();
  }

  std::cout << "[Direct Consumer] Total sum: " << data->received_sum << std::endl;
  return 0;
}

void example_direct_coroutine_context() {
  std::cout << "\n========== Example 4: Direct Coroutine Context Usage ==========\n" << std::endl;

  std::pair<copp::stackful_channel_receiver<int>, copp::stackful_channel_sender<int>> channel =
      copp::make_stackful_channel<int>();
  auto receiver = std::move(channel.first);
  auto sender = std::move(channel.second);

  direct_context_example_data data;
  data.receiver = std::move(receiver);

  // Create coroutine context directly
  typedef copp::coroutine_context_container<copp::allocator::stack_allocator_malloc> coroutine_t;

  coroutine_t::ptr_t co = coroutine_t::create(direct_consumer_runner);

  std::cout << "Starting direct coroutine..." << std::endl;
  co->start(&data);

  // Send values
  for (int value : {10, 20, 30}) {
    std::cout << "\nSending value: " << value << std::endl;
    sender.set_value(value);
  }

  std::cout << "\nDirect coroutine completed with sum: " << data.received_sum << std::endl;
}

// ===============================================
// Main
// ===============================================

int main() {
#if defined(LIBCOTASK_MACRO_ENABLED)
  example_basic_channel();
  example_error_handling();
  example_direct_coroutine_context();

  std::cout << "\n========== All Examples Completed! ==========\n" << std::endl;
#else
  std::cerr << "libcotask is not enabled, sample is disabled" << std::endl;
#endif
  return 0;
}
