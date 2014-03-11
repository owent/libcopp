#ifndef _COPP_COROUTINE_CONTEXT_MANAGER_H_
#define _COPP_COROUTINE_CONTEXT_MANAGER_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <map>
#include <memory>

#include <libcopp/utils/features.h>
#include <libcopp/utils/errno.h>
#include <libcopp/coroutine/coroutine_runnable_base.h>
#include <libcopp/coroutine/coroutine_context_safe_base.h>
#include <libcopp/coroutine/coroutine_context_container.h>
#include <libcopp/coroutine/core/standard_int_key_allocator.h>

namespace copp {
    class coroutine_manager_runner_base: public coroutine_runnable_base {
    public:
        // TODO events
    };

    namespace detail {
        template<typename TKey, typename TCOC, typename TStackAlloc>
        class coroutine_manager_node: public coroutine_context_container<TCOC, TStackAlloc> {
        public:
            typedef TKey key_type;
            typedef coroutine_context_container<TCOC, TStackAlloc> value_type;
            typedef value_type base_type;
            typedef coroutine_manager_runner_base runner_type;
            typedef runner_type* runner_ptr_type;
            typedef coroutine_manager_node<key_type, TCOC, TStackAlloc> self_type;

        public:
            coroutine_manager_node(key_type key): base_type(), key_(key){}
            virtual ~coroutine_manager_node() {
                _clear_runner();
            }

        private:
            void _clear_runner() {
                runner_ptr_type runner = dynamic_cast<runner_ptr_type>(base_type::get_runner());
                value_type::set_runner(NULL);
                if (NULL != runner)
                    delete runner;

                base_type::_reset_stack();
            }

        protected:

            virtual int set_runner(coroutine_runnable_base* runner) {
                _clear_runner();
                return value_type::set_runner(runner);
            }

        public:
            inline key_type get_key() const { return key_; }

            bool operator<(const self_type& right) const {
                return key_ < right.key_;
            }

            #if defined(COPP_MACRO_ENABLE_VARIADIC_TEMPLATE) && COPP_MACRO_ENABLE_VARIADIC_TEMPLATE
            template<typename TRunner, typename... TARGS>
            runner_ptr_type create_runner(std::size_t stack_size, TARGS... args) {
                runner_ptr_type ret = new TRunner(args...);
                _clear_runner();
                base_type::create(ret, stack_size);
                return ret;
            }
            #else
            template<typename TRunner>
            runner_ptr_type create_runner(std::size_t stack_size) {
                runner_ptr_type ret = new TRunner();
                _clear_runner();
                base_type::create(ret, stack_size);
                return ret;
            }
            template<typename TRunner, typename Arg0>
            runner_ptr_type create_runner(std::size_t stack_size, Arg0 arg0){
                runner_ptr_type ret = new TRunner(arg0);
                _clear_runner();
                base_type::create(ret, stack_size);
                return ret;
            }
            template<typename TRunner, typename Arg0, typename Arg1>
            runner_ptr_type create_runner(std::size_t stack_size, Arg0 arg0, Arg1 arg1){
                runner_ptr_type ret = new TRunner(arg0, arg1);
                _clear_runner();
                base_type::create(ret, stack_size);
                return ret;
            }
            template<typename TRunner, typename Arg0, typename Arg1, typename Arg2>
            runner_ptr_type create_runner(std::size_t stack_size, Arg0 arg0, Arg1 arg1, Arg2 arg2){
                runner_ptr_type ret = new TRunner(arg0, arg1, arg2);
                _clear_runner();
                base_type::create(ret, stack_size);
                return ret;
            }
            #endif

        private:
            key_type key_;
        };

        /**
         * data layer adapter
         */
        template <typename TKeyAlloc,
            typename TCOC = coroutine_context_safe_base,
            typename TStackAlloc = allocator::default_statck_allocator>
        class container_stl_map {
        public:
            typedef TKeyAlloc key_allocator_type;
            typedef typename key_allocator_type::value_type key_type;
            typedef coroutine_manager_node<key_type, TCOC, TStackAlloc> value_type;
            typedef value_type* value_ptr_type;
#if defined(COPP_MACRO_ENABLE_SMART_PTR) && COPP_MACRO_ENABLE_SMART_PTR
            typedef std::unique_ptr<value_type> value_ptr_type_s;
#else
            typedef std::auto_ptr<value_type> value_ptr_type_s;
#endif
            typedef std::map<key_type, value_ptr_type_s> data_type;
            typedef data_type self_type;
            typedef typename data_type::iterator iterator;
            typedef typename data_type::const_iterator const_iterator;

        public:
            /**
             * insert coroutine node
             * @param key key
             * @param val_ptr node pointer
             * @return true if success
             */
            bool insert(key_type key, value_ptr_type val_ptr)
            {
                if (NULL == val_ptr)
                    return false;

                typedef typename data_type::value_type pair_type;
                return data_.insert(pair_type(key, value_ptr_type_s(val_ptr))).second;
            }

            /**
             * find coroutine node by key
             * @param key
             * @return NULL or pointer of coroutine node
             */
            value_ptr_type find(key_type key) {
                iterator iter = data_.find(key);
                if (data_.end() == iter)
                    return NULL;
                return iter->second.get();
            }

            /**
             * find coroutine node by key
             * @param key
             * @return NULL or pointer of coroutine node
             */
            const value_ptr_type find(key_type key) const {
                const_iterator iter = data_.find(key);
                if (data_.end() == iter)
                    return NULL;
                return iter->second.get();
            }

            /**
             * remove coroutine node by key
             * @param key
             */
            void remove(key_type key) {
                data_.erase(key);
            }

            /**
             * get coroutine number
             * @return coroutine number
             */
            std::size_t size() const {
                return data_.size();
            }
        private:
            data_type data_;
        };
    }

    template<typename TContainer>
    class coroutine_mamanger {
    public:
        typedef TContainer container_type;
        typedef typename container_type::key_allocator_type key_allocator_type;
        typedef typename container_type::key_type key_type;
        typedef typename container_type::value_type value_type;
        typedef typename container_type::value_ptr_type value_ptr_type;

    public:

        coroutine_mamanger(){}

        value_ptr_type create() {
            key_type key = key_allocator_.allocate();
            while (NULL != container_.find(key))
                key = key_allocator_.allocate();

            value_ptr_type ret = new value_type(key);
            if (NULL == ret)
                return ret;

            bool is_success = container_.insert(key, ret);
            if (false == is_success)
                return NULL;

            return ret;
        }

        void remove(key_type key) {
            container_.remove(key);
            key_allocator_.deallocate(key);
        }

        value_ptr_type get_by_key(key_type key) {
            return container_.find(key);
        }

        const value_ptr_type get_by_key(key_type key) const {
            return container_.find(key);
        }

        std::size_t size() const {
            return container_.size();
        }

        // ============================================
        int start(key_type key) {
            value_ptr_type co = get_by_key(key);
            if (NULL == co)
                return COPP_EC_COROUTINE_NOT_FOUND;
            return co->start();
        }

        int resume(key_type key) {
            value_ptr_type co = get_by_key(key);
            if (NULL == co)
                return COPP_EC_COROUTINE_NOT_FOUND;
            return co->resume();
        }

    protected:
        container_type container_;
        key_allocator_type key_allocator_;
    };

    typedef coroutine_mamanger<
        detail::container_stl_map< detail::standard_int_key_allocator<uint64_t> >
    > default_coroutine_manager;
}

#endif
