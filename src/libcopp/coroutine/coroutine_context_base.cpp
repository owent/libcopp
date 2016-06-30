#include <algorithm>
#include <assert.h>
#include <cstring>

#include <libcopp/utils/errno.h>

#include <libcopp/coroutine/coroutine_context_base.h>

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

        static UTIL_CONFIG_THREAD_LOCAL coroutine_context_base *gt_current_coroutine = UTIL_CONFIG_NULLPTR;

#endif

        static void set_this_coroutine_context(coroutine_context_base *p) {
#ifndef UTIL_CONFIG_THREAD_LOCAL
            (void)pthread_once(&gt_coroutine_init_once, init_pthread_this_coroutine_context);
            pthread_setspecific(gt_coroutine_tls_key, p);
#else
            gt_current_coroutine = p;
#endif
        }

        static coroutine_context_base *get_this_coroutine_context() {
#ifndef UTIL_CONFIG_THREAD_LOCAL
            (void)pthread_once(&gt_coroutine_init_once, init_pthread_this_coroutine_context);
            return reinterpret_cast<coroutine_context_base *>(pthread_getspecific(gt_coroutine_tls_key));
#else

            return gt_current_coroutine;
#endif
        }


        coroutine_context_base::coroutine_context_base() UTIL_CONFIG_NOEXCEPT : runner_ret_code_(0),
                                                                                runner_(UTIL_CONFIG_NULLPTR),
                                                                                priv_data_(UTIL_CONFIG_NULLPTR),
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

        coroutine_context_base::~coroutine_context_base() { set_runner(UTIL_CONFIG_NULLPTR); }

        int coroutine_context_base::create(coroutine_runnable_base *runner,
                                           void (*func)(::copp::fcontext::transfer_t)) UTIL_CONFIG_NOEXCEPT {

            if (NULL == func) {
                func = &coroutine_context_base::coroutine_context_callback;
            }

            int from_status = status_t::EN_CRS_INVALID;
            if (false == status_.compare_exchange_strong(from_status, status_t::EN_CRS_READY)) {
                return COPP_EC_ALREADY_INITED;
            }

            coroutine_context_base::set_runner(runner);


            if (NULL == callee_stack_.sp || 0 == callee_stack_.size) return COPP_EC_NOT_INITED;

            callee_ = fcontext::copp_make_fcontext(callee_stack_.sp, callee_stack_.size, func);
            if (NULL == callee_) return COPP_EC_FCONTEXT_MAKE_FAILED;

            return COPP_EC_SUCCESS;
        }

        int coroutine_context_base::start(void *priv_data) {
            if (NULL == callee_) {
                return COPP_EC_NOT_INITED;
            }

            do {
                int from_status = status_.load();
                if (from_status < status_t::EN_CRS_READY) {
                    return COPP_EC_NOT_INITED;
                }

                from_status = status_t::EN_CRS_READY;
                if (status_.compare_exchange_strong(from_status, status_t::EN_CRS_RUNNING)) {
                    break;
                } else {
                    // finished or stoped
                    if (from_status > status_t::EN_CRS_RUNNING) {
                        return COPP_EC_NOT_READY;
                    }

                    // already running
                    if (status_t::EN_CRS_RUNNING == from_status) {
                        return COPP_EC_SUCCESS;
                    }
                }
            } while (true);

            jump_src_data_t jump_data;
            jump_data.from_co = get_this_coroutine_context();
            jump_data.to_co = this;
            jump_data.priv_data = priv_data;

#ifdef COPP_MACRO_USE_SEGMENTED_STACKS
            jump_to(callee_, caller_stack_, callee_stack_, jump_data);
#else
            jump_to(callee_, callee_stack_, callee_stack_, jump_data);
#endif

            return COPP_EC_SUCCESS;
        }

        int coroutine_context_base::yield(void **priv_data) {
            if (UTIL_CONFIG_NULLPTR == callee_) {
                return COPP_EC_NOT_INITED;
            }

            int from_status = status_t::EN_CRS_RUNNING;
            if (false == status_.compare_exchange_strong(from_status, status_t::EN_CRS_READY)) {
                if (status_t::EN_CRS_INVALID == from_status) {
                    return COPP_EC_NOT_INITED;
                } else if (status_t::EN_CRS_READY == from_status) {
                    return COPP_EC_NOT_RUNNING;
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

        int coroutine_context_base::resume(void *priv_data) { return start(priv_data); }

        int coroutine_context_base::set_runner(coroutine_runnable_base *runner) UTIL_CONFIG_NOEXCEPT {
            runner_ = runner;
            return COPP_EC_SUCCESS;
        }

        bool coroutine_context_base::is_finished() const UTIL_CONFIG_NOEXCEPT { return status_.load() >= status_t::EN_CRS_FINISHED; }

        void coroutine_context_base::jump_to(fcontext::fcontext_t &to_fctx, stack_context &from_sctx, stack_context &to_sctx,
                                             jump_src_data_t &jump_transfer) UTIL_CONFIG_NOEXCEPT {

            copp::fcontext::transfer_t res;
            jump_src_data_t *jump_src;

// can not use any more stack now

#ifdef COPP_MACRO_USE_SEGMENTED_STACKS
            assert(&from_sctx != &to_sctx);
            __splitstack_getcontext(from_sctx.segments_ctx);
            __splitstack_setcontext(to_sctx.segments_ctx);

            // ROOT->A: jump_transfer.from_co == NULL, jump_transfer.to_co == A, from_sctx == A.caller_stack_, skip backup segments
            // A->B.start(): jump_transfer.from_co == A, jump_transfer.to_co == B, from_sctx == B.caller_stack_, backup segments
            // B.yield()->A: jump_transfer.from_co == B, jump_transfer.to_co == NULL, from_sctx == B.callee_stack_, skip backup segments
            if (UTIL_CONFIG_NULLPTR != jump_transfer.from_co && (&from_sctx) != &jump_transfer.from_co->callee_stack_) {
                memcpy(&jump_transfer.from_co->callee_stack_.segments_ctx, &from_sctx.segments_ctx, sizeof(from_sctx.segments_ctx));
            }
#endif
            res = copp::fcontext::copp_jump_fcontext(to_fctx, &jump_transfer);
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
                int from_status = jump_src->from_co->status_.load();
                if (status_t::EN_CRS_RUNNING == from_status) {
                    jump_src->from_co->status_.compare_exchange_strong(from_status, status_t::EN_CRS_READY);
                } else if (status_t::EN_CRS_FINISHED == from_status) {
                    // if in finished status, change it to exited
                    jump_src->from_co->status_.store(status_t::EN_CRS_EXITED);
                }
            }

            // private data
            jump_transfer.priv_data = jump_src->priv_data;

            // this_coroutine
            set_this_coroutine_context(jump_transfer.from_co);
        }

        void coroutine_context_base::coroutine_context_callback(::copp::fcontext::transfer_t src_ctx) {
            assert(src_ctx.data);

            // copy jump_src_data_t in case it's destroyed later
            jump_src_data_t jump_src = *reinterpret_cast<jump_src_data_t *>(src_ctx.data);

            // this must in a coroutine
            coroutine_context_base *ins_ptr = jump_src.to_co;
            assert(ins_ptr);

            // update caller of to_co
            ins_ptr->caller_ = src_ctx.fctx;

            // save from_co's fcontext and switch status
            if (UTIL_CONFIG_NULLPTR != jump_src.from_co) {
                jump_src.from_co->callee_ = src_ctx.fctx;
                int from_status = status_t::EN_CRS_RUNNING; // from coroutine change status from running to ready
                jump_src.from_co->status_.compare_exchange_strong(from_status, status_t::EN_CRS_READY);
            }

            // this_coroutine
            set_this_coroutine_context(ins_ptr);

            // run logic code
            ins_ptr->run_and_recv_retcode(jump_src.priv_data);

            ins_ptr->status_.store(status_t::EN_CRS_FINISHED);
            // jump back to caller
            ins_ptr->yield();
        }
    }

    namespace this_coroutine {
        detail::coroutine_context_base *get_coroutine() UTIL_CONFIG_NOEXCEPT { return detail::get_this_coroutine_context(); }

        int yield(void **priv_data) {
            detail::coroutine_context_base *pco = get_coroutine();
            if (UTIL_CONFIG_NULLPTR != pco) {
                return pco->yield(priv_data);
            }

            return COPP_EC_NOT_RUNNING;
        }
    }
}
