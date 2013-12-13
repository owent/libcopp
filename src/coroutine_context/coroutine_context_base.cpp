#include <assert.h>
#include <cstring>

#include "libcopp/coroutine_context/coroutine_context_base.h"


#ifdef COPP_MACRO_USE_SEGMENTED_STACKS
extern "C" {
void __splitstack_getcontext( void* [COPP_MACRO_SEGMENTED_STACK_NUMBER]);

void __splitstack_setcontext( void* [COPP_MACRO_SEGMENTED_STACK_NUMBER]);

void __splitstack_releasecontext (void* [COPP_MACRO_SEGMENTED_STACK_NUMBER]);

void __splitstack_block_signals_context( void* [COPP_MACRO_SEGMENTED_STACK_NUMBER], int *, int *);

}
#endif

namespace copp { 
    namespace detail{
        coroutine_context_base::coroutine_context_base() :
            caller_(), callee_(NULL), preserve_fpu_(true), callee_stack_()
#ifdef COPP_MACRO_USE_SEGMENTED_STACKS
            ,caller_stack_()
#endif
        {
        }


        coroutine_context_base::~coroutine_context_base()
        {
        }

        int coroutine_context_base::create(char* stack_ptr, size_t stack_len, void(*func)(intptr_t))
        {
            callee_stack_.sp = stack_ptr;
            callee_stack_.size = stack_len;

            return create(func);
        }

        int coroutine_context_base::create(void(*func)(intptr_t))
        {
            if (NULL == callee_stack_.sp || 0 == callee_stack_.size)
                return COPP_EC_NOT_INITED;

            callee_ = fcontext::copp_make_fcontext(callee_stack_.sp, callee_stack_.size, func);
            if (NULL == callee_)
                return COPP_EC_FCONTEXT_MAKE_FAILED;

            return COPP_EC_SUCCESS;
        }

        int coroutine_context_base::start()
        {
            if (NULL == callee_)
                return COPP_EC_NOT_INITED;

#ifdef COPP_MACRO_USE_SEGMENTED_STACKS
            jump_to(caller_, *callee_, caller_stack_, callee_stack_, (intptr_t) this, preserve_fpu_);
#else
            jump_to(caller_, *callee_, callee_stack_, callee_stack_, (intptr_t) this, preserve_fpu_);
#endif

            return COPP_EC_SUCCESS;
        }

        int coroutine_context_base::yield()
        {
            if (NULL == callee_)
                return COPP_EC_NOT_INITED;

#ifdef COPP_MACRO_USE_SEGMENTED_STACKS
            jump_to(*callee_, caller_, callee_stack_, caller_stack_, (intptr_t) this, preserve_fpu_);
#else
            jump_to(*callee_, caller_, callee_stack_, callee_stack_, (intptr_t) this, preserve_fpu_);
#endif

            return COPP_EC_SUCCESS;
        }

        int coroutine_context_base::resume()
        {
            return start();
        }

        intptr_t coroutine_context_base::jump_to(fcontext::fcontext_t& from_fcontext, const fcontext::fcontext_t& to_fcontext,
            stack_context& from_stack, stack_context& to_stack,
            intptr_t param, bool preserve_fpu)
        {
#ifdef COPP_MACRO_USE_SEGMENTED_STACKS
            assert(&from_stack != &to_stack);
            __splitstack_getcontext( from_stack.segments_ctx);
            __splitstack_setcontext( to_stack.segments_ctx);
#endif
            intptr_t ret = copp::fcontext::copp_jump_fcontext(&from_fcontext, &to_fcontext, param, preserve_fpu);
            return ret;
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
