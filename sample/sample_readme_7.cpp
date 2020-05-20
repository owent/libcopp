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

// static copp::future::task_t<int> call_for_await_cotask() {
//     co_return 0;
// }

int main() {
    return 0;
}
#else
int main() {
    puts("this sample require cotask enabled and compiler support c++20 coroutine");
    return 0;
}
#endif
