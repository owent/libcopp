#include <cstring>

#include <libcopp/coroutine/coroutine_context_base.h>
#include <libcopp/coroutine/coroutine_runnable_base.h>

namespace copp {
    namespace detail {
        coroutine_runnable_base::coroutine_runnable_base() {}

        coroutine_runnable_base::~coroutine_runnable_base() {}

        int coroutine_runnable_base::operator()() { return 0; }

        int coroutine_runnable_base::operator()(void *priv_data) { return (*this)(); }
    }
}
