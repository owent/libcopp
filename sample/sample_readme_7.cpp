/*
 * sample_readme_7.cpp
 *
 *  Created on: 2020-05-20
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

int main() {
    copp::future::task_t<int> t1 = call_for_coroutine_task_with_int_result();
    copp::future::task_t<void> t2 = call_for_coroutine_task_with_void_result();
    std::cout<< "Coroutine t1: "<< t1.get_task_id()<< " -> "<< *t1.data()<< std::endl;
    std::cout<< "Coroutine t2: "<< t2.get_task_id()<< " -> "<< (t2.done()? "done": "running")<< std::endl;
    return 0;
}
#else
int main() {
    puts("this sample require cotask enabled and compiler support c++20 coroutine");
    return 0;
}
#endif
