# USAGE

> Just include headers and linking library file of your platform to use libcopp

## Example
There is a simple example of using coroutine manager below:

```cpp
#include <cstdio>
#include <cstring>
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
        return 0;
    }
};

int main() {
	// create a coroutine manager
	copp::default_coroutine_manager co_mgr;
	typedef copp::default_coroutine_manager::value_type co_type;
	
	// create a coroutine object
	co_type* pco = g_co_mgr.create();
	// create and bind runner to coroutine object
	pco->create_runner<my_runner>(64 * 1024); // allocate 64KB stack memory

	// start a coroutine
	co_mgr.start(pco->get_key());

	// destroy a coroutine [optional], all coroutine will be destroyed automatically when co_mgr is destroyed
	co_mgr.remove(pco->get_key()); // pco is invalid now.
	return 0;
}

```

And then, you can custom many function such as set your id type, stack allocator, coroutine type and etc. by set your template parameters of coroutine manager.

