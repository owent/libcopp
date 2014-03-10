#include <assert.h>
#include <cstring>

#include "libcopp/coroutine/coroutine_context_safe_base.h"

namespace copp { 
    namespace detail{
        coroutine_context_safe_base::coroutine_context_safe_base() :
            coroutine_context_base(), status_busy_(), status_running_(EN_CRS_INVALID)
        {
        }


        coroutine_context_safe_base::~coroutine_context_safe_base()
        {
        }

        int coroutine_context_safe_base::create(coroutine_runnable_base* runner, char* stack_ptr, size_t stack_len, void(*func)(intptr_t))
        {
            if (NULL == func)
                func = &coroutine_context_safe_base::coroutine_context_callback;

            if (EN_CRS_INVALID != status_running_)
                return COPP_EC_ALREADY_INITED;

            // enter critical section
            if (false == status_busy_.try_lock())
                return COPP_EC_ACCESS_VIOLATION;

            int ret = COPP_EC_SUCCESS;
            do {
                // check again
                if (EN_CRS_INVALID != status_running_) {
                    ret = COPP_EC_ALREADY_INITED;
                    break;
                }

                ret = coroutine_context_base::create(runner, stack_ptr, stack_len, func);
                if (COPP_EC_SUCCESS == ret)
                    status_running_ = EN_CRS_START;

            } while (false);
            status_busy_.unlock();

            return ret;
        }

        int coroutine_context_safe_base::create(coroutine_runnable_base* runner, void(*func)(intptr_t))
        {
            if (NULL == func)
                func = &coroutine_context_safe_base::coroutine_context_callback;

            if (EN_CRS_INVALID != status_running_)
                return COPP_EC_ALREADY_INITED;

            // enter critical section
            if (false == status_busy_.try_lock())
                return COPP_EC_ACCESS_VIOLATION;

            int ret = COPP_EC_SUCCESS;
            do {
                // check again
                if (EN_CRS_INVALID != status_running_) {
                    ret = COPP_EC_ALREADY_INITED;
                    break;
                }

                ret = coroutine_context_base::create(runner, func);
                if (COPP_EC_SUCCESS == ret)
                    status_running_ = EN_CRS_START;

            } while (false);
            status_busy_.unlock();

            return ret;
        }

        int coroutine_context_safe_base::start()
        {
            if ( status_running_ < EN_CRS_START )
                return COPP_EC_NOT_INITED;

            if (status_running_ > EN_CRS_RUNNING)
                return COPP_EC_NOT_READY;

            // critical section
            if (false == status_busy_.try_lock())
                return COPP_EC_SUCCESS;

            int ret = COPP_EC_SUCCESS;
            do {
                // check again
                if (status_running_ > EN_CRS_RUNNING) {
                    ret = COPP_EC_NOT_READY;
                    break;
                }

                ret = coroutine_context_base::start();

                if (EN_CRS_FINISHED == status_running_)
                    status_running_ = EN_CRS_STOP;

                return ret;
            } while (false);

            status_busy_.unlock();
            return ret;
        }

        int coroutine_context_safe_base::yield()
        {
            if (EN_CRS_RUNNING != status_running_ && EN_CRS_FINISHED != status_running_)
                return COPP_EC_NOT_READY;

            // critical section
            if (false == status_busy_.try_unlock())
                return COPP_EC_NOT_RUNNING;

            int ret = COPP_EC_SUCCESS;
            do {
                // check again
                if (EN_CRS_RUNNING != status_running_ && EN_CRS_FINISHED != status_running_) {
                    ret = COPP_EC_NOT_READY;
                    break;
                }

                ret = coroutine_context_base::yield();

                return ret;
            } while (false);

            status_busy_.lock();
            return ret;
        }

        int coroutine_context_safe_base::resume()
        {
            return start();
        }

        int coroutine_context_safe_base::stop()
        {
            if (status_running_ < EN_CRS_START)
                return COPP_EC_NOT_INITED;

            if (status_running_ > EN_CRS_RUNNING)
                return COPP_EC_NOT_READY;

            // critical section
            if (false == status_busy_.try_lock())
                return COPP_EC_ACCESS_VIOLATION;

            status_running_ = EN_CRS_STOP;
            status_busy_.unlock();

            return COPP_EC_SUCCESS;
        }

        int coroutine_context_safe_base::set_runner(coroutine_runnable_base* runner)
        {
            if (status_running_ < EN_CRS_START)
                return COPP_EC_NOT_INITED;

            if (EN_CRS_RUNNING == status_running_)
                return COPP_EC_IS_RUNNING;

            // double check, critical section
            if (false == status_busy_.try_lock())
                return COPP_EC_IS_RUNNING;

            int ret = coroutine_context_base::set_runner(runner);

            // quit critical section
            status_busy_.unlock();

            return ret;
        }

        void coroutine_context_safe_base::coroutine_context_callback(intptr_t coro_ptr)
        {
            coroutine_context_safe_base* ins_ptr = (coroutine_context_safe_base*) coro_ptr;

            ins_ptr->status_running_ = EN_CRS_RUNNING;
            // run logic code
            ins_ptr->run();
            ins_ptr->status_running_ = EN_CRS_FINISHED;

            // directly jump back to caller, do not check running status
            ins_ptr->yield();
        }
    }
}
