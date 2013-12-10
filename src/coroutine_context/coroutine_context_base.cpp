#include <cstring>

#include "libcopp/coroutine_context/coroutine_context_base.h"

namespace copp { 
    namespace detail{
        coroutine_context_base::coroutine_context_base() :
            caller_(), callee_(NULL), env_stack_(), preserve_fpu_(true)
        {
        }


        coroutine_context_base::~coroutine_context_base()
        {
        }

        int coroutine_context_base::create(char* stack_ptr, size_t stack_len, void(*func)(intptr_t))
        {
            env_stack_.sp = stack_ptr;
            env_stack_.size = stack_len;

            return create(func);
        }

        int coroutine_context_base::create(void(*func)(intptr_t))
        {
            if (NULL == env_stack_.sp || 0 == env_stack_.size)
                return COPP_EC_NOT_INITED;

            callee_ = fcontext::copp_make_fcontext(env_stack_.sp, env_stack_.size, func);
            if (NULL == callee_)
                return COPP_EC_FCONTEXT_MAKE_FAILED;

            return COPP_EC_SUCCESS;
        }

        int coroutine_context_base::start()
        {
            if (NULL == callee_)
                return COPP_EC_NOT_INITED;

            copp::fcontext::copp_jump_fcontext(&caller_, callee_, (intptr_t) this, preserve_fpu_);

            return COPP_EC_SUCCESS;
        }

        int coroutine_context_base::yield()
        {
            if (NULL == callee_)
                return COPP_EC_NOT_INITED;

            copp::fcontext::copp_jump_fcontext(callee_, &caller_, (intptr_t) this, preserve_fpu_);

            return COPP_EC_SUCCESS;
        }

        int coroutine_context_base::resume()
        {
            return start();
        }

        void coroutine_context_base::coroutine_context_callback(intptr_t coro_ptr)
        {
            coroutine_context_base* ins_ptr = (coroutine_context_base*) coro_ptr;

            // run logic code
            ins_ptr->run();

            // jump back to caller
            ins_ptr->yield();
        }
    }
}
