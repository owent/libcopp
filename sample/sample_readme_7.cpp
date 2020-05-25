/*
 * sample_readme_7.cpp
 *
 *  Created on: 2020-05-25
 *      Author: owent
 *
 *  Released under the MIT license
 */

#include <iostream>

// include manager header file
#include <libcopp/future/std_coroutine_task.h>

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

static copp::future::task_t<int> call_for_coroutine_task_with_int_result() {
    // ... any code
    co_return 123;
}

static copp::future::task_t<void> call_for_coroutine_task_with_void_result() {
    // ... any code
    co_return;
}

struct sample_task_waker_t;
typedef copp::future::task_t<int, sample_task_waker_t>    sample_task_t;
typedef copp::future::task_future_t<int>                  sample_future_t;
typedef copp::future::task_context_t<sample_task_waker_t> sample_task_context_t;

std::list<std::pair<sample_task_context_t *,int> > g_sample_executor;

struct sample_task_waker_t {
    std::list<std::pair<sample_task_context_t *,int> >::iterator refer_to;

    sample_task_waker_t() {
        refer_to = g_sample_executor.end();
    }

    ~sample_task_waker_t() {
        if (refer_to != g_sample_executor.end()) {
            g_sample_executor.erase(refer_to);
        }
    }

    void operator()(sample_future_t &fut, sample_task_context_t &ctx) {
        if (refer_to == g_sample_executor.end()) {
            // Add to custom executor when first polled
            refer_to = g_sample_executor.insert(
                g_sample_executor.end(), 
                std::make_pair(&ctx, 0)
            );
            return;
        }

        if (0 != (*refer_to).second) {
            fut.poll_data() = (*refer_to).second;
            // Because return type is a trivial type, we can just assign to value
            // It the return type is a trivial type, we can use 
            //     fut.poll_data() = copp::future::make_unique<T>(...);
            //   or 
            //     fut.poll_data() = std::make_unique<T>(...);
            //   to set the result data.
            g_sample_executor.erase(refer_to);
            refer_to = g_sample_executor.end();
        }
    }
};

static sample_task_t call_for_coroutine_task_with_custom_waker() {
    // suspend and wait custom waker
    co_await LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE suspend_always();
    // ... any code
    // We can get the pointer to the future and th context of current task by co_yield current_future and current_context
    sample_future_t* future = co_yield sample_task_t::current_future();
    sample_task_context_t* context = co_yield sample_task_t::current_context();
    if (future && context && future->is_ready()) {
        // The return value will be ignored when the future is already set by custom waker
        std::cout<< "Coroutine: "<< context->get_task_id()<< " already got future data "<< (*future->data())<< " and will ignore co_return."<< std::endl;
    }
    co_return 123;
}

int main() {
    copp::future::task_t<int> t1 = call_for_coroutine_task_with_int_result();
    copp::future::task_t<void> t2 = call_for_coroutine_task_with_void_result();
    sample_task_t t3 = call_for_coroutine_task_with_custom_waker();
    std::cout<< "Coroutine t1: "<< t1.get_task_id()<< " -> "<< *t1.data()<< std::endl;
    std::cout<< "Coroutine t2: "<< t2.get_task_id()<< " -> "<< (t2.done()? "done": "running")<< std::endl;

    while (!g_sample_executor.empty()) {
        (*g_sample_executor.begin()).second = 456;
        (*g_sample_executor.begin()).first->wake();
    }

    std::cout<< "Coroutine t3: "<< t3.get_task_id()<< " -> "<< *t3.data()<< std::endl;
    return 0;
}
#else
int main() {
    puts("this sample require cotask enabled and compiler support c++20 coroutine");
    return 0;
}
#endif
