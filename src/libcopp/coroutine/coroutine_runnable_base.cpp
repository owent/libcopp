#include <cstring>

#include <libcopp/coroutine/coroutine_runnable_base.h>
#include <libcopp/coroutine/coroutine_context_base.h>

namespace copp { 
    namespace detail{
        coroutine_runnable_base::coroutine_runnable_base():
            coroutine_context_(NULL)
        {
        }

        coroutine_runnable_base::~coroutine_runnable_base()
        {
            if (NULL != coroutine_context_ && this == coroutine_context_->runner_)
                coroutine_context_->runner_ = NULL;
        }
    }
}
