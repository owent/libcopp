#include <algorithm>
#include <assert.h>
#include <cstdlib>
#include <cstring>

#include <libcopp/utils/config/compiler_features.h>
#include <libcopp/utils/config/libcopp_build_features.h>
#include <libcopp/utils/errno.h>
#include <libcopp/utils/std/explicit_declare.h>

#include <libcopp/coroutine/coroutine_context_base.h>

#if defined(UTIL_CONFIG_THREAD_LOCAL)
// using thread_local
#else
#include <pthread.h>
#endif

namespace copp {
    namespace detail {

#if defined(LIBCOPP_DISABLE_THIS_MT) && LIBCOPP_DISABLE_THIS_MT
        static coroutine_context_base *gt_current_coroutine = UTIL_CONFIG_NULLPTR;
#elif defined(UTIL_CONFIG_THREAD_LOCAL)
        static UTIL_CONFIG_THREAD_LOCAL coroutine_context_base *gt_current_coroutine = UTIL_CONFIG_NULLPTR;
#else
        static pthread_once_t gt_coroutine_init_once = PTHREAD_ONCE_INIT;
        static pthread_key_t  gt_coroutine_tls_key;
        static void           init_pthread_this_coroutine_context() { (void)pthread_key_create(&gt_coroutine_tls_key, UTIL_CONFIG_NULLPTR); }
#endif

        static inline void set_this_coroutine_context(coroutine_context_base *p) {
#if (defined(LIBCOPP_DISABLE_THIS_MT) && LIBCOPP_DISABLE_THIS_MT) || defined(UTIL_CONFIG_THREAD_LOCAL)
            gt_current_coroutine = p;
#else
            (void)pthread_once(&gt_coroutine_init_once, init_pthread_this_coroutine_context);
            pthread_setspecific(gt_coroutine_tls_key, p);
#endif
        }

        static inline coroutine_context_base *get_this_coroutine_context() {
#if (defined(LIBCOPP_DISABLE_THIS_MT) && LIBCOPP_DISABLE_THIS_MT) || defined(UTIL_CONFIG_THREAD_LOCAL)
            return gt_current_coroutine;
#else
            (void)pthread_once(&gt_coroutine_init_once, init_pthread_this_coroutine_context);
            return reinterpret_cast<coroutine_context_base *>(pthread_getspecific(gt_coroutine_tls_key));
#endif
        }
    } // namespace detail

    LIBCOPP_COPP_API coroutine_context_base::coroutine_context_base() LIBCOPP_MACRO_NOEXCEPT : runner_ret_code_(0),
                                                                                   flags_(0),
                                                                                   runner_(UTIL_CONFIG_NULLPTR),
                                                                                   priv_data_(UTIL_CONFIG_NULLPTR),
                                                                                   private_buffer_size_(0),
                                                                                   status_(status_t::EN_CRS_INVALID) {
    }

    LIBCOPP_COPP_API coroutine_context_base::~coroutine_context_base() {}

    LIBCOPP_COPP_API bool coroutine_context_base::set_flags(int flags) LIBCOPP_MACRO_NOEXCEPT {
        if (flags & flag_t::EN_CFT_MASK) {
            return false;
        }

        flags_ |= flags;
        return true;
    }

    LIBCOPP_COPP_API bool coroutine_context_base::unset_flags(int flags) LIBCOPP_MACRO_NOEXCEPT {
        if (flags & flag_t::EN_CFT_MASK) {
            return false;
        }

        flags_ &= ~flags;
        return true;
    }

    LIBCOPP_COPP_API bool coroutine_context_base::check_flags(int flags) const LIBCOPP_MACRO_NOEXCEPT { return 0 != (flags_ & flags); }

#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
    LIBCOPP_COPP_API int coroutine_context_base::set_runner(callback_t &&runner) {
#else
    LIBCOPP_COPP_API int coroutine_context_base::set_runner(const callback_t &runner) {
#endif
        if (!runner) {
            return COPP_EC_ARGS_ERROR;
        }

        int from_status = status_t::EN_CRS_INVALID;
        if (false == status_.compare_exchange_strong(from_status, status_t::EN_CRS_READY, libcopp::util::lock::memory_order_acq_rel,
                                                     libcopp::util::lock::memory_order_acquire)) {
            return COPP_EC_ALREADY_INITED;
        }

        runner_ = std::move(runner);
        return COPP_EC_SUCCESS;
    } // namespace copp

    LIBCOPP_COPP_API bool coroutine_context_base::is_finished() const LIBCOPP_MACRO_NOEXCEPT {
        // return !!(flags_ & flag_t::EN_CFT_FINISHED);
        return status_.load(libcopp::util::lock::memory_order_acquire) >= status_t::EN_CRS_FINISHED;
    }

    LIBCOPP_COPP_API coroutine_context_base* coroutine_context_base::get_this_coroutine_base() LIBCOPP_MACRO_NOEXCEPT {
        return detail::get_this_coroutine_context();
    }

    LIBCOPP_COPP_API void coroutine_context_base::set_this_coroutine_base(coroutine_context_base * ctx) LIBCOPP_MACRO_NOEXCEPT {
        detail::set_this_coroutine_context(ctx);
    }
} // namespace copp
