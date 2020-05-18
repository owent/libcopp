#include <ctime>

#if defined(THREAD_TLS_USE_PTHREAD) && THREAD_TLS_USE_PTHREAD
#include <pthread.h>
#endif

#include <libcopp/utils/atomic_int_type.h>
#include <libcopp/utils/uint64_id_allocator.h>

namespace copp {
    namespace util {
        namespace details {

            // TIMESTAMP:32|SEQUENCE:24 -> about 16M id per second
            static uint64_t allocate_id_by_atomic() {
                static libcopp::util::lock::atomic_int_type<uint64_t> seq_alloc(255);

                // 
#if defined(UTIL_CONFIG_COMPILER_CXX_CONSTEXPR) && UTIL_CONFIG_COMPILER_CXX_CONSTEXPR
                static constexpr size_t   seq_bits  = 24;
                static constexpr uint64_t time_mask = (static_cast<uint64_t>(1) << 32) - 1;
#else
                static const size_t   seq_bits  = 24;
                static const uint64_t time_mask = (static_cast<uint64_t>(1) << 32) - 1;
#endif


                // always do not allocate 0 as a valid ID
                uint64_t ret = 0;
                while (0 == ret) {
                    uint64_t res       = seq_alloc.load();
                    uint64_t time_part = res >> seq_bits;

                    uint64_t next_ret       = res + 1;
                    uint64_t next_time_part = next_ret >> seq_bits;
                    if (0 == time_part || time_part != next_time_part) {
                        uint64_t now_time = time_part;
                        while (time_part == now_time) {
                            now_time = (static_cast<uint64_t>(time(NULL)) & time_mask) - 1577836800; // 2020-01-01 00:00:00+00:00 UTC
                        }

                        // if failed, maybe another thread do it
                        if (seq_alloc.compare_exchange_strong(res, now_time << seq_bits, libcopp::util::lock::memory_order_acq_rel,
                                                              libcopp::util::lock::memory_order_acquire)) {
                            ret = now_time << seq_bits;
                        }
                    } else {
                        if (seq_alloc.compare_exchange_weak(res, next_ret, libcopp::util::lock::memory_order_acq_rel,
                                                            libcopp::util::lock::memory_order_acquire)) {
                            ret = next_ret;
                        }
                    }
                }

                return ret;
            }

            struct uint64_id_allocator_tls_cache_t {
                uint64_t base;
                uint64_t inner_seq;
            };

#if defined(THREAD_TLS_USE_PTHREAD) && THREAD_TLS_USE_PTHREAD
            static pthread_once_t gt_uint64_id_allocator_tls_once = PTHREAD_ONCE_INIT;
            static pthread_key_t  gt_uint64_id_allocator_tls_key;

            static void dtor_pthread_uint64_id_allocator_tls(void *p) {
                uint64_id_allocator_tls_cache_t *cache = reinterpret_cast<uint64_id_allocator_tls_cache_t *>(p);
                if (NULL != cache) {
                    delete cache;
                }
            }

            static void init_pthread_uint64_id_allocator_tls() { (void)pthread_key_create(&gt_uint64_id_allocator_tls_key, dtor_pthread_uint64_id_allocator_tls); }

            uint64_id_allocator_tls_cache_t *get_uint64_id_allocator_tls_cache() {
                (void)pthread_once(&gt_uint64_id_allocator_tls_once, init_pthread_uint64_id_allocator_tls);
                uint64_id_allocator_tls_cache_t *ret = reinterpret_cast<uint64_id_allocator_tls_cache_t *>(pthread_getspecific(gt_uint64_id_allocator_tls_key));
                if (NULL == ret) {
                    ret = new uint64_id_allocator_tls_cache_t();
                    ret->base = 0;
                    ret->inner_seq = 0;
                    pthread_setspecific(gt_uint64_id_allocator_tls_key, ret);
                }
                return ret;
            }

            struct gt_uint64_id_allocator_tls_cache_main_thread_dtor_t {
                uint64_id_allocator_tls_cache_t* cache_ptr;
                gt_uint64_id_allocator_tls_cache_main_thread_dtor_t(){
                    cache_ptr = get_uint64_id_allocator_tls_cache();
                }

                ~gt_uint64_id_allocator_tls_cache_main_thread_dtor_t() {
                    pthread_setspecific(gt_uint64_id_allocator_tls_key, NULL);
                    dtor_pthread_uint64_id_allocator_tls(reinterpret_cast<uint64_id_allocator_tls_cache_t *>(p));
                }
            };
            static gt_uint64_id_allocator_tls_cache_main_thread_dtor_t gt_uint64_id_allocator_tls_cache_main_thread_dtor;
#else
            uint64_id_allocator_tls_cache_t *get_uint64_id_allocator_tls_cache() {
                static thread_local uint64_id_allocator_tls_cache_t ret = { 0, 0 };
                return &ret;
            }
#endif
        }

        LIBCOPP_COPP_API uint64_id_allocator::value_type uint64_id_allocator::allocate() UTIL_CONFIG_NOEXCEPT {
            // details::allocate_id_by_atomic() takes 56 bits, we use 5 bits here
            details::uint64_id_allocator_tls_cache_t * tls_cache = details::get_uint64_id_allocator_tls_cache();
            if (NULL == tls_cache) {
                return 0;
            }

#if defined(UTIL_CONFIG_COMPILER_CXX_CONSTEXPR) && UTIL_CONFIG_COMPILER_CXX_CONSTEXPR
            static constexpr uint64_t tls_cache_count = (static_cast<uint64_t>(1) << 5);
#else
            static const uint64_t tls_cache_count = (static_cast<uint64_t>(1) << 5);
#endif

            while (0 == tls_cache->base || tls_cache->inner_seq >= tls_cache_count) {
                tls_cache->base = details::allocate_id_by_atomic();
                tls_cache->inner_seq = 0;
            }

            return (tls_cache->base << 5) + (tls_cache->inner_seq ++);
        }

        LIBCOPP_COPP_API void uint64_id_allocator::deallocate(value_type) UTIL_CONFIG_NOEXCEPT {}
    }
}
