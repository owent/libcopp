/*
 * sample_default_manager.cpp
 *
 *  Created on: 2014年3月17日
 *      Author: owent
 *
 *  Released under the MIT license
 */


#include <cstdio>
#include <cstring>
#include <inttypes.h>
#include <stdint.h>

// include manager header file
#include <libcopp/coroutine/coroutine_manager.h>

// define a coroutine runner
class my_runner : public copp::coroutine_manager_runner_base
{
public:
    typedef copp::default_coroutine_manager::value_type value_type;
public:
    int operator()() {
        // ... your code here ...
        uint64_t key = get_coroutine_context<copp::default_coroutine_manager::value_type>()->get_key();
        printf("cortoutine %" PRIu64 " is running.\n", key);
        return 1;
    }
};

int main() {
    // create a coroutine manager
    copp::default_coroutine_manager co_mgr;
    typedef copp::default_coroutine_manager::value_type co_type;

    // create a coroutine object
    co_type* pco = co_mgr.create();
    // create and bind runner to coroutine object
    pco->create_runner<my_runner>(64 * 1024); // allocate 64KB stack memory

    // start a coroutine
    co_mgr.start(pco->get_key());
    printf("cortoutine %" PRIu64 " exit and return %d.\n", pco->get_key(), pco->get_ret_code());

    // destroy a coroutine [optional], all coroutine will be destroyed automatically when co_mgr is destroyed
    co_mgr.remove(pco->get_key()); // pco is invalid now.
    return 0;
}

