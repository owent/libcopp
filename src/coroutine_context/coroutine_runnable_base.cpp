#include <cstring>

#include <libcopp/coroutine_context/coroutine_runnable_base.h>
#include <libcopp/coroutine_context/coroutine_context_base.h>

namespace copp { 
    namespace detail{
        coroutine_runnable_base::coroutine_runnable_base():
            coroutine_context_(NULL)
        {
        }

        coroutine_runnable_base::~coroutine_runnable_base()
        {
            if (NULL != coroutine_context_)
                coroutine_context_->runner_ = NULL;
        }
    }
}
