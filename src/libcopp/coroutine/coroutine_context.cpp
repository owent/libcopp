#include <algorithm>
#include <assert.h>
#include <cstdlib>
#include <cstring>

#include <libcopp/utils/errno.h>

#include <libcopp/coroutine/coroutine_context.h>

#ifndef UTIL_CONFIG_THREAD_LOCAL

#include <pthread.h>

#endif

#ifdef COPP_MACRO_USE_SEGMENTED_STACKS
extern "C" {
void __splitstack_getcontext(void * [COPP_MACRO_SEGMENTED_STACK_NUMBER]);

void __splitstack_setcontext(void * [COPP_MACRO_SEGMENTED_STACK_NUMBER]);

void __splitstack_releasecontext(void * [COPP_MACRO_SEGMENTED_STACK_NUMBER]);

void __splitstack_block_signals_context(void * [COPP_MACRO_SEGMENTED_STACK_NUMBER], int *, int *);
}
#endif

namespace copp {
    namespace detail {

#ifndef UTIL_CONFIG_THREAD_LOCAL

        static pthread_once_t gt_coroutine_init_once = PTHREAD_ONCE_INIT;
        static pthread_key_t gt_coroutine_tls_key;
        static void init_pthread_this_coroutine_context() { (void)pthread_key_create(&gt_coroutine_tls_key, UTIL_CONFIG_NULLPTR); }

#else

        static UTIL_CONFIG_THREAD_LOCAL coroutine_context *gt_current_coroutine = UTIL_CONFIG_NULLPTR;

#endif

        static void set_this_coroutine_context(coroutine_context *p) {
#ifndef UTIL_CONFIG_THREAD_LOCAL
            (void)pthread_once(&gt_coroutine_init_once, init_pthread_this_coroutine_context);
            pthread_setspecific(gt_coroutine_tls_key, p);
#else
            gt_current_coroutine = p;
#endif
        }

        static coroutine_context *get_this_coroutine_context() {
#ifndef UTIL_CONFIG_THREAD_LOCAL
            (void)pthread_once(&gt_coroutine_init_once, init_pthread_this_coroutine_context);
            return reinterpret_cast<coroutine_context *>(pthread_getspecific(gt_coroutine_tls_key));
#else

            return gt_current_coroutine;
#endif
        }
    }

    coroutine_context::coroutine_context() UTIL_CONFIG_NOEXCEPT : runner_ret_code_(0),
                                                                  flags_(0),
                                                                  runner_(UTIL_CONFIG_NULLPTR),
                                                                  priv_data_(UTIL_CONFIG_NULLPTR),
                                                                  private_buffer_size_(0),
                                                                  status_(status_t::EN_CRS_INVALID),
                                                                  caller_(UTIL_CONFIG_NULLPTR),
                                                                  callee_(UTIL_CONFIG_NULLPTR),
                                                                  callee_stack_()
#ifdef COPP_MACRO_USE_SEGMENTED_STACKS
                                                                      ,
                                                                  caller_stack_()
#endif
    {
    }

    coroutine_context::~coroutine_context() {}

    int coroutine_context::create(coroutine_context *p, callback_t &runner, const stack_context &callee_stack, size_t coroutine_size,
                                  size_t private_buffer_size) UTIL_CONFIG_NOEXCEPT {
        if (UTIL_CONFIG_NULLPTR == p) {
            return COPP_EC_ARGS_ERROR;
        }

        // must aligned to sizeof(size_t)
        if (0 != (private_buffer_size & (sizeof(size_t) - 1))) {
            return COPP_EC_ARGS_ERROR;
        }

        if (0 != (coroutine_size & (sizeof(size_t) - 1))) {
            return COPP_EC_ARGS_ERROR;
        }

        size_t stack_offset = private_buffer_size + coroutine_size;
        if (NULL == callee_stack.sp || callee_stack.size <= stack_offset) {
            return COPP_EC_ARGS_ERROR;
        }

        // stack down
        // |STARCK BUFFER........COROUTINE..this..padding..PRIVATE DATA.....callee_stack.sp|
        // |------------------------------callee_stack.size -------------------------------|
        if (callee_stack.sp <= p || coroutine_size < sizeof(coroutine_context)) {
            return COPP_EC_ARGS_ERROR;
        }

        size_t this_offset = reinterpret_cast<unsigned char *>(callee_stack.sp) - reinterpret_cast<unsigned char *>(p);
        if (this_offset < sizeof(coroutine_context) + private_buffer_size || this_offset > stack_offset) {
            return COPP_EC_ARGS_ERROR;
        }

        // if runner is empty, we can set it later
        p->set_runner(COPP_MACRO_STD_MOVE(runner));

        if (&p->callee_stack_ != &callee_stack) {
            p->callee_stack_ = callee_stack;
        }
        p->private_buffer_size_ = private_buffer_size;

        // stack down, left enough private data
        p->priv_data_ = reinterpret_cast<unsigned char *>(p->callee_stack_.sp) - p->private_buffer_size_;
        p->callee_ = fcontext::copp_make_fcontext(reinterpret_cast<unsigned char *>(p->callee_stack_.sp) - stack_offset,
                                                  p->callee_stack_.size - stack_offset, &coroutine_context::coroutine_context_callback);
        if (NULL == p->callee_) {
            return COPP_EC_FCONTEXT_MAKE_FAILED;
        }

        return COPP_EC_SUCCESS;
    }

    int coroutine_context::start(void *priv_data) {
        if (NULL == callee_) {
            return COPP_EC_NOT_INITED;
        }

        int from_status = status_t::EN_CRS_READY;
        do {
            if (from_status < status_t::EN_CRS_READY) {
                return COPP_EC_NOT_INITED;
            }

            if (status_.compare_exchange_strong(from_status, status_t::EN_CRS_RUNNING, util::lock::memory_order_acq_rel,
                                                util::lock::memory_order_acquire)) {
                break;
            } else {
                // finished or stoped
                if (from_status > status_t::EN_CRS_RUNNING) {
                    return COPP_EC_NOT_READY;
                }

                // already running
                if (status_t::EN_CRS_RUNNING == from_status) {
                    return COPP_EC_IS_RUNNING;
                }
            }
        } while (true);

        jump_src_data_t jump_data;
        jump_data.from_co = detail::get_this_coroutine_context();
        jump_data.to_co = this;
        jump_data.priv_data = priv_data;

#ifdef COPP_MACRO_USE_SEGMENTED_STACKS
        jump_to(callee_, caller_stack_, callee_stack_, jump_data);
#else
        jump_to(callee_, callee_stack_, callee_stack_, jump_data);
#endif

        // [BUG #4](https://github.com/owt5008137/libcopp/issues/4)
        // Move changing status to the end of start(private data)
        {
            // assume it's running, or set into EN_CRS_EXITED if in EN_CRS_FINISHED
            from_status = status_t::EN_CRS_RUNNING;
            if (false ==
                status_.compare_exchange_strong(from_status, status_t::EN_CRS_READY, util::lock::memory_order_acq_rel,
                                                util::lock::memory_order_acquire)) {
                if (status_t::EN_CRS_FINISHED == from_status) {
                    // if in finished status, change it to exited
                    status_.store(status_t::EN_CRS_EXITED, util::lock::memory_order_release);
                }
            }
        }

        return COPP_EC_SUCCESS;
    }

    int coroutine_context::resume(void *priv_data) { return start(priv_data); }

    int coroutine_context::yield(void **priv_data) {
        if (UTIL_CONFIG_NULLPTR == callee_) {
            return COPP_EC_NOT_INITED;
        }

        int from_status = status_t::EN_CRS_RUNNING;
        if (false ==
            status_.compare_exchange_strong(from_status, status_t::EN_CRS_READY, util::lock::memory_order_acq_rel,
                                            util::lock::memory_order_acquire)) {
            switch (from_status) {
            case status_t::EN_CRS_INVALID:
                return COPP_EC_NOT_INITED;
            case status_t::EN_CRS_READY:
                return COPP_EC_NOT_RUNNING;
            case status_t::EN_CRS_FINISHED:
                break;
            case status_t::EN_CRS_EXITED:
                return COPP_EC_ALREADY_EXIST;
            default:
                return COPP_EC_UNKNOWN;
            }
        }

        // success or finished will continue
        jump_src_data_t jump_data;
        jump_data.from_co = this;
        jump_data.to_co = UTIL_CONFIG_NULLPTR;


#ifdef COPP_MACRO_USE_SEGMENTED_STACKS
        jump_to(caller_, callee_stack_, caller_stack_, jump_data);
#else
        jump_to(caller_, callee_stack_, callee_stack_, jump_data);
#endif

        if (UTIL_CONFIG_NULLPTR != priv_data) {
            *priv_data = jump_data.priv_data;
        }

        return COPP_EC_SUCCESS;
    }

    int coroutine_context::set_runner(const callback_t &runner) {
        if (!runner) {
            return COPP_EC_ARGS_ERROR;
        }

        int from_status = status_t::EN_CRS_INVALID;
        if (false ==
            status_.compare_exchange_strong(from_status, status_t::EN_CRS_READY, util::lock::memory_order_acq_rel,
                                            util::lock::memory_order_acquire)) {
            return COPP_EC_ALREADY_INITED;
        }

        runner_ = runner;
        return COPP_EC_SUCCESS;
    }

#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
    int coroutine_context::set_runner(callback_t &&runner) {
        if (!runner) {
            return COPP_EC_ARGS_ERROR;
        }

        int from_status = status_t::EN_CRS_INVALID;
        if (false ==
            status_.compare_exchange_strong(from_status, status_t::EN_CRS_READY, util::lock::memory_order_acq_rel,
                                            util::lock::memory_order_acquire)) {
            return COPP_EC_ALREADY_INITED;
        }

        runner_ = COPP_MACRO_STD_MOVE(runner);
        return COPP_EC_SUCCESS;
    }
#endif

    bool coroutine_context::is_finished() const UTIL_CONFIG_NOEXCEPT {
        // return !!(flags_ & flag_t::EN_CFT_FINISHED);
        return status_.load(util::lock::memory_order_acquire) >= status_t::EN_CRS_FINISHED;
    }

    void coroutine_context::jump_to(fcontext::fcontext_t &to_fctx, stack_context &from_sctx, stack_context &to_sctx,
                                    jump_src_data_t &jump_transfer) UTIL_CONFIG_NOEXCEPT {

        copp::fcontext::transfer_t res;
        jump_src_data_t *jump_src;
// int from_status;
// bool swap_success;
// can not use any more stack now
// can not initialize those vars here

#ifdef COPP_MACRO_USE_SEGMENTED_STACKS
        assert(&from_sctx != &to_sctx);
        // ROOT->A: jump_transfer.from_co == NULL, jump_transfer.to_co == A, from_sctx == A.caller_stack_, skip backup segments
        // A->B.start(): jump_transfer.from_co == A, jump_transfer.to_co == B, from_sctx == B.caller_stack_, backup segments
        // B.yield()->A: jump_transfer.from_co == B, jump_transfer.to_co == NULL, from_sctx == B.callee_stack_, skip backup segments
        if (UTIL_CONFIG_NULLPTR != jump_transfer.from_co) {
            __splitstack_getcontext(jump_transfer.from_co->callee_stack_.segments_ctx);
            if (&from_sctx != &jump_transfer.from_co->callee_stack_) {
                memcpy(&from_sctx.segments_ctx, &jump_transfer.from_co->callee_stack_.segments_ctx, sizeof(from_sctx.segments_ctx));
            }
        } else {
            __splitstack_getcontext(from_sctx.segments_ctx);
        }
        __splitstack_setcontext(to_sctx.segments_ctx);
#endif
        res = copp::fcontext::copp_jump_fcontext(to_fctx, &jump_transfer);
        if (NULL == res.data) {
            abort();
            return;
        }
        jump_src = reinterpret_cast<jump_src_data_t *>(res.data);
        assert(jump_src);

        /**
            * save from_co's fcontext and switch status
            * we should use from_co in transfer_t, because it may not jump from jump_transfer.to_co
            *
            * if we jump sequence is A->B->C->A.resume(), and if this call is A->B, then
            * jump_src->from_co = C, jump_src->to_co = A, jump_transfer.from_co = A, jump_transfer.to_co = B
            * and now we should save the callee of C and set the caller of A = C
            *
            * if we jump sequence is A->B.yield()->A, and if this call is A->B, then
            * jump_src->from_co = B, jump_src->to_co = NULL, jump_transfer.from_co = A, jump_transfer.to_co = B
            * and now we should save the callee of B and should change the caller of A
            *
            */

        // update caller of to_co if not jump from yield mode
        if (UTIL_CONFIG_NULLPTR != jump_src->to_co) {
            jump_src->to_co->caller_ = res.fctx;
        }

        if (UTIL_CONFIG_NULLPTR != jump_src->from_co) {
            jump_src->from_co->callee_ = res.fctx;
            // [BUG #4](https://github.com/owt5008137/libcopp/issues/4)
            // from_status = jump_src->from_co->status_.load();
            // if (status_t::EN_CRS_RUNNING == from_status) {
            //     jump_src->from_co->status_.compare_exchange_strong(from_status, status_t::EN_CRS_READY, util::lock::memory_order_acq_rel,
            //     util::lock::memory_order_acquire);
            // } else if (status_t::EN_CRS_FINISHED == from_status) {
            //     // if in finished status, change it to exited
            //     jump_src->from_co->status_.store(status_t::EN_CRS_EXITED);
            // }
        }

        // private data
        jump_transfer.priv_data = jump_src->priv_data;

        // this_coroutine
        detail::set_this_coroutine_context(jump_transfer.from_co);

        // [BUG #4](https://github.com/owt5008137/libcopp/issues/4)
        // // resume running status of from_co
        // if (NULL != jump_transfer.from_co) {
        //     from_status = jump_transfer.from_co->status_.load();
        //     swap_success = false;
        //     while (!swap_success && status_t::EN_CRS_READY == from_status) {
        //         swap_success = jump_transfer.from_co->status_.compare_exchange_strong(from_status, status_t::EN_CRS_RUNNING,
        //         util::lock::memory_order_acq_rel, util::lock::memory_order_acquire);
        //     }
        // }
    }

    void coroutine_context::coroutine_context_callback(::copp::fcontext::transfer_t src_ctx) {
        assert(src_ctx.data);
        if (NULL == src_ctx.data) {
            abort();
            return;
        }

        // copy jump_src_data_t in case it's destroyed later
        jump_src_data_t jump_src = *reinterpret_cast<jump_src_data_t *>(src_ctx.data);

        // this must in a coroutine
        coroutine_context *ins_ptr = jump_src.to_co;
        assert(ins_ptr);
        if (NULL == ins_ptr) {
            abort();
            return;
        }

        // update caller of to_co
        ins_ptr->caller_ = src_ctx.fctx;

        // save from_co's fcontext and switch status
        if (UTIL_CONFIG_NULLPTR != jump_src.from_co) {
            jump_src.from_co->callee_ = src_ctx.fctx;
            // [BUG #4](https://github.com/owt5008137/libcopp/issues/4)
            // int from_status = status_t::EN_CRS_RUNNING; // from coroutine change status from running to ready
            // jump_src.from_co->status_.compare_exchange_strong(from_status, status_t::EN_CRS_READY, util::lock::memory_order_acq_rel,
            // util::lock::memory_order_acquire);
        }

        // this_coroutine
        detail::set_this_coroutine_context(ins_ptr);

        // run logic code
        ins_ptr->run_and_recv_retcode(jump_src.priv_data);

        ins_ptr->flags_ |= flag_t::EN_CFT_FINISHED;
        ins_ptr->status_.store(status_t::EN_CRS_FINISHED, util::lock::memory_order_release);
        // add memory fence to flush flags_(used in is_finished())
        // UTIL_LOCK_ATOMIC_THREAD_FENCE(util::lock::memory_order_release);

        // jump back to caller
        ins_ptr->yield();
    }

    namespace this_coroutine {
        coroutine_context *get_coroutine() UTIL_CONFIG_NOEXCEPT { return detail::get_this_coroutine_context(); }

        int yield(void **priv_data) {
            coroutine_context *pco = get_coroutine();
            if (UTIL_CONFIG_NULLPTR != pco) {
                return pco->yield(priv_data);
            }

            return COPP_EC_NOT_RUNNING;
        }
    }
}
