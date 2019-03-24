#include <iostream>

// include task header file
#include <libcotask/task.h>

typedef cotask::task<> my_task_t;

int main() {
#if defined(UTIL_CONFIG_COMPILER_CXX_LAMBDAS) && UTIL_CONFIG_COMPILER_CXX_LAMBDAS
    // create a task using factory function [with lambda expression]
    my_task_t::ptr_t task = my_task_t::create([]() {
        std::cout << "task " << cotask::this_task::get<my_task_t>()->get_id() << " started" << std::endl;
        cotask::this_task::get_task()->yield();
        std::cout << "task " << cotask::this_task::get<my_task_t>()->get_id() << " resumed" << std::endl;
        return 0;
    });

    std::cout << "task " << task->get_id() << " created" << std::endl;
    // start a task
    task->start();

    std::cout << "task " << task->get_id() << " yield" << std::endl;
    task->resume();
    std::cout << "task " << task->get_id() << " stoped, ready to be destroyed." << std::endl;
#else
    std::cerr << "lambda not supported, this sample is not available." << std::endl;
#endif
    return 0;
}
