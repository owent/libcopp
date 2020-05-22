/*
 * sample_readme_8.cpp
 *
 *  Created on: 2020-05-22
 *      Author: owent
 *
 *  Released under the MIT license
 */

#include <assert.h>
#include <iostream>
#include <string>

// include manager header file
#include <libcopp/future/std_coroutine_task.h>
#include <libcopp/future/std_coroutine_generator.h>

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

struct sample_message_t {
    int ret_code;
    std::string response;
};

struct sample_generator_waker_t;

typedef copp::future::result_t<sample_message_t, int32_t> sample_result_t;
typedef copp::future::task_t<sample_result_t>             sample_task_t;
typedef copp::future::generator_future_t<sample_result_t> sample_future_t;
typedef copp::future::generator_context_t<sample_generator_waker_t> sample_generator_context_t;


std::list<std::pair<sample_generator_context_t *, std::string> > g_sample_executor;

struct sample_generator_waker_t {
    int32_t code;
    std::list<std::pair<sample_generator_context_t *, std::string> >::iterator refer_to;

    // All parameter passed into generator will be forward here
    sample_generator_waker_t(int32_t c) : code(c) {
        refer_to = g_sample_executor.end();
    }

    ~sample_generator_waker_t() {
        if (refer_to != g_sample_executor.end()) {
            g_sample_executor.erase(refer_to);
        }
    }

    void operator()(sample_future_t &fut, sample_generator_context_t &ctx) {
        if (refer_to == g_sample_executor.end()) {
            // Add to custom executor when first polled
            refer_to = g_sample_executor.insert(
                g_sample_executor.end(), 
                std::make_pair(&ctx, std::string())
            );
            return;
        }

        if (!(*refer_to).second.empty()) {
            // generator finished and produce a result message
            sample_message_t msg;
            msg.ret_code    = code;
            msg.response.swap((*refer_to).second);
            fut.poll_data() = sample_result_t::make_success(msg);

            g_sample_executor.erase(refer_to);
            refer_to = g_sample_executor.end();
        }
    }
};
typedef copp::future::generator_t<sample_result_t, sample_generator_waker_t> sample_generator_t;

static copp::future::task_t<int> call_for_coroutine_task() {
    sample_generator_t generator = copp::future::make_generator<sample_generator_t>(200);
    auto result = co_await generator;

    if (result) {
        if (result->is_success()) {
            std::cout<< "Got response message: "<< result->get_success()->response<< std::endl;
            co_return result->get_success()->ret_code;    
        } else {
            co_return *result->get_error();
        }
    }
    co_return 0;
}

int main() {
    copp::future::task_t<int> t = call_for_coroutine_task();
    assert(false == t.done());
    assert(NULL == t.data());  // Task isn't finished and has no data

    while (!g_sample_executor.empty()) {
        // async jobs finished and wake coroutine here
        g_sample_executor.begin()->second = "Hello World!";
        g_sample_executor.begin()->first->wake();
    }

    assert(t.done());
    assert(t.data()); // Task is finished and has data
    std::cout<< "Task "<< t.get_task_id()<< " finished and got result: "<< *t.data()<< std::endl;
    return 0;
}
#else
int main() {
    puts("this sample require cotask enabled and compiler support c++20 coroutine");
    return 0;
}
#endif
