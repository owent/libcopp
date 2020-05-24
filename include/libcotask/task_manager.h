/*
 * task_manager.h
 *
 *  Created on: 2014年6月16日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef COTASK_TASK_MANAGER_H
#define COTASK_TASK_MANAGER_H

#pragma once

#include <algorithm>
#include <assert.h>
#include <ctime>
#include <set>
#include <stdint.h>
#include <vector>
#include <list>

#ifdef __cpp_impl_three_way_comparison
#include <compare>
#endif

#if (defined(__cplusplus) && __cplusplus >= 201103L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201103L)
#include <unordered_map>
#define COTASK_MACRO_MANAGER_USING_UNORDERED_MAP 1
#else
#include <map>
#endif

#include <libcotask/task_macros.h>

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
#include <exception>
#endif

namespace cotask {

    namespace detail {
        struct LIBCOPP_COTASK_API_HEAD_ONLY tickspec_t {
            time_t tv_sec;  /* Seconds.  */
            int    tv_nsec; /* Nanoseconds.  */

            friend bool operator==(const tickspec_t &l, const tickspec_t &r) { return l.tv_sec == r.tv_sec && l.tv_nsec == r.tv_nsec; }

#ifdef __cpp_impl_three_way_comparison
            friend std::strong_ordering operator<=>(const tickspec_t &l, const tickspec_t &r) { 
                return (l.tv_sec != r.tv_sec) ? l.tv_sec <=> r.tv_sec : l.tv_nsec <=> r.tv_nsec;
            }
#else
            friend bool operator!=(const tickspec_t &l, const tickspec_t &r) { return l.tv_sec != r.tv_sec || l.tv_nsec != r.tv_nsec; }

            friend bool operator<(const tickspec_t &l, const tickspec_t &r) {
                return (l.tv_sec != r.tv_sec) ? l.tv_sec < r.tv_sec : l.tv_nsec < r.tv_nsec;
            }

            friend bool operator<=(const tickspec_t &l, const tickspec_t &r) {
                return (l.tv_sec != r.tv_sec) ? l.tv_sec <= r.tv_sec : l.tv_nsec <= r.tv_nsec;
            }

            friend bool operator>(const tickspec_t &l, const tickspec_t &r) {
                return (l.tv_sec != r.tv_sec) ? l.tv_sec > r.tv_sec : l.tv_nsec > r.tv_nsec;
            }

            friend bool operator>=(const tickspec_t &l, const tickspec_t &r) {
                return (l.tv_sec != r.tv_sec) ? l.tv_sec >= r.tv_sec : l.tv_nsec >= r.tv_nsec;
            }
#endif
        };

        template <typename TTask>
        struct LIBCOPP_COTASK_API_HEAD_ONLY task_timer_node {
            tickspec_t           expired_time;
            typename TTask::id_t task_id;

            friend bool operator==(const task_timer_node &l, const task_timer_node &r) {
                return l.expired_time == r.expired_time && l.task_id == r.task_id;
            }

#ifdef __cpp_impl_three_way_comparison
            friend std::strong_ordering operator<=>(const task_timer_node &l, const task_timer_node &r) {
                if (l.expired_time != r.expired_time) {
                    return l.expired_time <=> r.expired_time;
                }

                return l.task_id <=> r.task_id;
            }
#else
            friend bool operator!=(const task_timer_node &l, const task_timer_node &r) {
                return l.expired_time != r.expired_time || l.task_id != r.task_id;
            }

            friend bool operator<(const task_timer_node &l, const task_timer_node &r) {
                if (l.expired_time != r.expired_time) {
                    return l.expired_time < r.expired_time;
                }

                return l.task_id < r.task_id;
            }

            friend bool operator<=(const task_timer_node &l, const task_timer_node &r) {
                if (l.expired_time != r.expired_time) {
                    return l.expired_time <= r.expired_time;
                }

                return l.task_id <= r.task_id;
            }

            friend bool operator>(const task_timer_node &l, const task_timer_node &r) {
                if (l.expired_time != r.expired_time) {
                    return l.expired_time > r.expired_time;
                }

                return l.task_id > r.task_id;
            }

            friend bool operator>=(const task_timer_node &l, const task_timer_node &r) {
                if (l.expired_time != r.expired_time) {
                    return l.expired_time >= r.expired_time;
                }

                return l.task_id >= r.task_id;
            }
#endif
        };

        template <typename TTask>
        struct LIBCOPP_COTASK_API_HEAD_ONLY task_manager_node {
            typedef typename TTask::ptr_t task_ptr_t;

            task_ptr_t                                           task_;
            typename std::set<task_timer_node<TTask> >::iterator timer_node;
        };

    } // namespace detail

    /**
     * @brief task manager
     */
    template <typename TTask,
#if defined(COTASK_MACRO_MANAGER_USING_UNORDERED_MAP)
              typename TTaskContainer = std::unordered_map<typename TTask::id_t, detail::task_manager_node<TTask> >
#else
              typename TTaskContainer = std::map<typename TTask::id_t, detail::task_manager_node<TTask> >
#endif
              >
    class LIBCOPP_COTASK_API_HEAD_ONLY task_manager {
    public:
        typedef TTask                             task_t;
        typedef TTaskContainer                    container_t;
        typedef typename task_t::id_t             id_t;
        typedef typename task_t::ptr_t            task_ptr_t;
        typedef task_manager<task_t, container_t> self_t;
        typedef std::shared_ptr<self_t>           ptr_t;

        struct flag_t {
            enum type {
                EN_TM_NONE     = 0x00,
                EN_TM_IN_TICK  = 0x01,
                EN_TM_IN_RESET = 0x02,
            };
        };

    private:
        struct flag_guard_t {
            int *                 data_;
            typename flag_t::type flag_;
            inline flag_guard_t(int *flags, typename flag_t::type v) : data_(flags), flag_(v) {
                if (NULL == data_ || (*data_ & flag_)) {
                    flag_ = flag_t::EN_TM_NONE;
                    data_ = NULL;
                } else {
                    (*data_) |= flag_;
                }
            }
            inline ~flag_guard_t() {
                if (*this) {
                    (*data_) &= ~flag_;
                }
            }

            inline operator bool() { return NULL != data_ && flag_t::EN_TM_NONE != flag_; }
        };

    public:
        task_manager() : flags_(0) {
            last_tick_time_.tv_sec  = 0;
            last_tick_time_.tv_nsec = 0;
        }

        ~task_manager() {
            // safe remove all task
            reset();
        }

        void reset() {
            flag_guard_t reset_flag(&flags_, flag_t::EN_TM_IN_RESET);
            if (!reset_flag) {
                return;
            }

            std::vector<task_ptr_t> all_tasks;
            // first, lock and reset all data
            {
#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
                libcopp::util::lock::lock_holder<libcopp::util::lock::spin_lock> lock_guard(action_lock_);
#endif

                for (typename container_t::iterator iter = tasks_.begin(); iter != tasks_.end(); ++iter) {
                    all_tasks.push_back(iter->second.task_);
                    remove_timeout_timer(iter->second);
                }

                tasks_.clear();
                task_timeout_timer_.clear();
                flags_                  = 0;
                last_tick_time_.tv_sec  = 0;
                last_tick_time_.tv_nsec = 0;
            }

            // then, kill all tasks
            for (typename std::vector<task_ptr_t>::iterator iter = all_tasks.begin(); iter != all_tasks.end(); ++iter) {
                if (!(*iter)->is_exiting()) {
                    (*iter)->kill(EN_TS_KILLED);
                }
            }
        }

        /**
         * @brief create a new task manager
         * @return smart pointer of task manager
         */
        static ptr_t create() { return std::make_shared<self_t>(); }

        /**
         * @brief add task to manager
         *        please make the task has method of get_id() and will return a unique id
         *
         * @param task task to be inserted
         * @param timeout_sec timeout in second ( unix time stamp recommanded )
         * @param timeout_nsec timeout in nanosecond ( must be in the range 0-999999999 )
         * @return 0 or error code
         *
         * @note if a task added before the first calling of tick method,
         *       the timeout will be set releative to the first calling time of tick method
         * @see tick
         */
        int add_task(const task_ptr_t &task, time_t timeout_sec, int timeout_nsec) {
            if (!task) {
                assert(task);
                return copp::COPP_EC_ARGS_ERROR;
            }

            if (flags_ & flag_t::EN_TM_IN_RESET) {
                return copp::COPP_EC_IN_RESET;
            }

            if (task->is_exiting()) {
                return copp::COPP_EC_TASK_IS_EXITING;
            }

            // try to cast type
            typedef typename container_t::value_type pair_type;
            detail::task_manager_node<task_t>        task_node;
            task_node.task_      = task;
            task_node.timer_node = task_timeout_timer_.end();

            if (!task_node.task_) {
                assert(task_node.task_);
                return copp::COPP_EC_CAST_FAILED;
            }

            // lock before we will operator tasks_
#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
            libcopp::util::lock::lock_holder<libcopp::util::lock::spin_lock> lock_guard(action_lock_);
#endif

            id_t task_id = task->get_id();
            if (tasks_.end() != tasks_.find(task_id)) {
                return copp::COPP_EC_ALREADY_EXIST;
            }

#if defined(LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER) && LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER
            if (!task_t::task_manager_helper::setup_task_manager(*task, reinterpret_cast<void *>(this), &task_cleanup_callback)) {
                return copp::COPP_EC_TASK_ALREADY_IN_ANOTHER_MANAGER;
            }
#endif

            // try to insert to container
            std::pair<typename container_t::iterator, bool> res = tasks_.insert(pair_type(task_id, task_node));
            if (false == res.second) {
#if defined(LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER) && LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER
                task_t::task_manager_helper::cleanup_task_manager(*task, reinterpret_cast<void *>(this));
#endif
                return copp::COPP_EC_EXTERNAL_INSERT_FAILED;
            }

            // add timeout controller
            set_timeout_timer(res.first->second, timeout_sec, timeout_nsec);
            return copp::COPP_EC_SUCCESS;
        }

        /**
         * @brief add task to manager
         *        please make the task has method of get_id() and will return a unique id
         *
         * @param task task to be inserted
         * @return 0 or error code
         *
         */
        int add_task(const task_ptr_t &task) { return add_task(task, 0, 0); }

        /**
         * @brief set or update task timeout
         *
         * @param id task id of which should be already added into this manager
         * @param timeout_sec timeout in second ( unix time stamp recommanded )
         * @param timeout_nsec timeout in nanosecond ( must be in the range 0-999999999 )
         * @return 0 or error code
         *
         * @note if a task added before the first calling of tick method,
         *       the timeout will be set releative to the first calling time of tick method
         * @note this function can be call multiply times, we will use the last timeout.
         *       set_timeout(TASK_ID, 0, 0) means the task with TASK_ID will never expire.
         * @see tick
         */
        int set_timeout(id_t id, time_t timeout_sec, int timeout_nsec) {
            if (flags_ & flag_t::EN_TM_IN_RESET) {
                return copp::COPP_EC_IN_RESET;
            }

            {
#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
                libcopp::util::lock::lock_holder<libcopp::util::lock::spin_lock> lock_guard(action_lock_);
#endif

                typedef typename container_t::iterator iter_type;
                iter_type                              iter = tasks_.find(id);
                if (tasks_.end() == iter) return copp::COPP_EC_NOT_FOUND;

                set_timeout_timer(iter->second, timeout_sec, timeout_nsec);
            }

            return copp::COPP_EC_SUCCESS;
        }

        /**
         * @brief remove task in this manager
         * @param id task id
         * @param confirm_ptr check task ptr before just remove by id
         * @return 0 or error code
         */
        inline int remove_task(id_t id, const task_ptr_t &confirm_ptr) { return remove_task(id, confirm_ptr.get()); }

        /**
         * @brief remove task in this manager
         * @param id task id
         * @return 0 or error code
         */
        inline int remove_task(id_t id) { return remove_task(id, UTIL_CONFIG_NULLPTR); }

        /**
         * @brief remove task in this manager
         * @param id task id
         * @param confirm_ptr check task ptr before just remove by id
         * @return 0 or error code
         */
        int remove_task(id_t id, const task_t *confirm_ptr) {
            if (flags_ & flag_t::EN_TM_IN_RESET) {
                return copp::COPP_EC_IN_RESET;
            }

            task_ptr_t task_inst;
            {
#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
                libcopp::util::lock::lock_holder<libcopp::util::lock::spin_lock> lock_guard(action_lock_);
#endif

                typedef typename container_t::iterator iter_type;
                iter_type                              iter = tasks_.find(id);
                if (tasks_.end() == iter) {
                    return copp::COPP_EC_NOT_FOUND;
                }
                if (UTIL_CONFIG_NULLPTR != confirm_ptr && iter->second.task_.get() != confirm_ptr) {
                    return copp::COPP_EC_NOT_FOUND;
                }

                // make sure running task be killed first
                task_inst = std::move(iter->second.task_);

                remove_timeout_timer(iter->second);
                tasks_.erase(iter);
            }

            if (task_inst) {
#if defined(LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER) && LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER
                // already cleanup, there is no need to cleanup again
                task_t::task_manager_helper::cleanup_task_manager(*task_inst, reinterpret_cast<void *>(this));
#endif

                EN_TASK_STATUS task_status = task_inst->get_status();
                if (task_status > EN_TS_CREATED && task_status < EN_TS_DONE) {
                    return task_inst->kill(EN_TS_KILLED, NULL);
                }
            }

            return copp::COPP_EC_SUCCESS;
        }

        /**
         * @brief find task by id
         * @param id task id
         * @return smart pointer of task
         */
        task_ptr_t find_task(id_t id) {
            if (flags_ & flag_t::EN_TM_IN_RESET) {
                return task_ptr_t();
            }

#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
            libcopp::util::lock::lock_holder<libcopp::util::lock::spin_lock> lock_guard(action_lock_);
#endif

            typedef typename container_t::iterator iter_type;
            iter_type                              iter = tasks_.find(id);
            if (tasks_.end() == iter) return task_ptr_t();

            return iter->second.task_;
        }

        // int add_scheduler();
        // int scheduling_once();
        // int scheduling_loop();
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
        int start(id_t id, void *priv_data = NULL) {
            std::list<std::exception_ptr> eptrs;
            int ret = start(id, eptrs, priv_data);
            task_t::maybe_rethrow(eptrs);
            return ret;
        }

        int start(id_t id, std::list<std::exception_ptr>& unhandled, void *priv_data = NULL) LIBCOPP_MACRO_NOEXCEPT {
#else
        int start(id_t id, void *priv_data = NULL) {
#endif
            if (flags_ & flag_t::EN_TM_IN_RESET) {
                return copp::COPP_EC_IN_RESET;
            }

            task_ptr_t task_inst;
            {
#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
                libcopp::util::lock::lock_holder<libcopp::util::lock::spin_lock> lock_guard(action_lock_);
#endif

                typedef typename container_t::iterator iter_type;
                iter_type                              iter = tasks_.find(id);
                if (tasks_.end() == iter) return copp::COPP_EC_NOT_FOUND;

                task_inst = iter->second.task_;
            }

            // unlock and then run start
            if (task_inst) {
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
                int ret = task_inst->start(unhandled, priv_data);
#else
                int ret = task_inst->start(priv_data);
#endif

                // if task is finished, remove it
                if (task_inst->get_status() >= EN_TS_DONE) {
                    remove_task(id);
                }

                return ret;
            } else {
                return copp::COPP_EC_NOT_FOUND;
            }
        }

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
        int resume(id_t id, void *priv_data = NULL) {
            std::list<std::exception_ptr> eptrs;
            int ret = resume(id, eptrs, priv_data);
            task_t::maybe_rethrow(eptrs);
            return ret;
        }

        int resume(id_t id, std::list<std::exception_ptr>& unhandled, void *priv_data = NULL) LIBCOPP_MACRO_NOEXCEPT {
#else
        int resume(id_t id, void *priv_data = NULL) {
#endif
            if (flags_ & flag_t::EN_TM_IN_RESET) {
                return copp::COPP_EC_IN_RESET;
            }

            task_ptr_t task_inst;
            {
#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
                libcopp::util::lock::lock_holder<libcopp::util::lock::spin_lock> lock_guard(action_lock_);
#endif

                typedef typename container_t::iterator iter_type;
                iter_type                              iter = tasks_.find(id);
                if (tasks_.end() == iter) return copp::COPP_EC_NOT_FOUND;

                task_inst = iter->second.task_;
            }

            // unlock and then run resume
            if (task_inst) {
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
                int ret = task_inst->resume(unhandled, priv_data);
#else
                int ret = task_inst->resume(priv_data);
#endif

                // if task is finished, remove it
                if (task_inst->get_status() >= EN_TS_DONE) {
                    remove_task(id);
                }

                return ret;
            } else {
                return copp::COPP_EC_NOT_FOUND;
            }
        }

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
        int cancel(id_t id, void *priv_data = NULL) {
            std::list<std::exception_ptr> eptrs;
            int ret = cancel(id, eptrs, priv_data);
            task_t::maybe_rethrow(eptrs);
            return ret;
        }

        int cancel(id_t id, std::list<std::exception_ptr>& unhandled, void *priv_data = NULL) LIBCOPP_MACRO_NOEXCEPT {
#else
        int cancel(id_t id, void *priv_data = NULL) {
#endif
            if (flags_ & flag_t::EN_TM_IN_RESET) {
                return copp::COPP_EC_IN_RESET;
            }

            task_ptr_t task_inst;
            {
#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
                libcopp::util::lock::lock_holder<libcopp::util::lock::spin_lock> lock_guard(action_lock_);
#endif

                typedef typename container_t::iterator iter_type;
                iter_type                              iter = tasks_.find(id);
                if (tasks_.end() == iter) {
                    return copp::COPP_EC_NOT_FOUND;
                }

                task_inst = std::move(iter->second.task_);

                remove_timeout_timer(iter->second);
                tasks_.erase(iter); // remove from container
            }

            // unlock and then run cancel
            if (task_inst) {
#if defined(LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER) && LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER
                // already cleanup, there is no need to cleanup again
                task_t::task_manager_helper::cleanup_task_manager(*task_inst, reinterpret_cast<void *>(this));
#endif
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
                return task_inst->cancel(unhandled, priv_data);
#else
                return task_inst->cancel(priv_data);
#endif
            } else {
                return copp::COPP_EC_NOT_FOUND;
            }
        }

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
        int kill(id_t id, enum EN_TASK_STATUS status, void *priv_data = NULL) {
            std::list<std::exception_ptr> eptrs;
            int ret = kill(id, eptrs, status, priv_data);
            task_t::maybe_rethrow(eptrs);
            return ret;
        }

        int kill(id_t id, std::list<std::exception_ptr>& unhandled, enum EN_TASK_STATUS status, void *priv_data = NULL) LIBCOPP_MACRO_NOEXCEPT {
#else
        int kill(id_t id, enum EN_TASK_STATUS status, void *priv_data = NULL) {
#endif
            if (flags_ & flag_t::EN_TM_IN_RESET) {
                return copp::COPP_EC_IN_RESET;
            }

            task_ptr_t task_inst;
            {
#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
                libcopp::util::lock::lock_holder<libcopp::util::lock::spin_lock> lock_guard(action_lock_);
#endif

                typedef typename container_t::iterator iter_type;
                iter_type                              iter = tasks_.find(id);
                if (tasks_.end() == iter) {
                    return copp::COPP_EC_NOT_FOUND;
                }

                task_inst = std::move(iter->second.task_);

                remove_timeout_timer(iter->second);
                tasks_.erase(iter); // remove from container
            }

            // unlock and then run kill
            if (task_inst) {
#if defined(LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER) && LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER
                // already cleanup, there is no need to cleanup again
                task_t::task_manager_helper::cleanup_task_manager(*task_inst, reinterpret_cast<void *>(this));
#endif
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
                return task_inst->kill(unhandled, status, priv_data);
#else
                return task_inst->kill(status, priv_data);
#endif
            } else {
                return copp::COPP_EC_NOT_FOUND;
            }
        }

        int kill(id_t id, void *priv_data = NULL) { return kill(id, EN_TS_KILLED, priv_data); }

        /**
         * @brief active tick event and deal with clock
         * @param sec current time in second ( unix time stamp recommanded )
         * @param nsec current time in nanosecond ( must be in the range 0-999999999 )
         * @return 0 or error code
         *
         * @note timeout tasks will be removed here
         */
        int tick(time_t sec, int nsec = 0) {
            detail::tickspec_t now_tick_time;
            // time can not be back
            if (sec < last_tick_time_.tv_sec || (sec == last_tick_time_.tv_sec && nsec <= last_tick_time_.tv_nsec)) {
                return 0;
            }

            now_tick_time.tv_sec  = sec;
            now_tick_time.tv_nsec = nsec;

            // we will ignore tick when in a recursive call
            flag_guard_t tick_flag(&flags_, flag_t::EN_TM_IN_TICK);
            if (!tick_flag) {
                return copp::COPP_EC_SUCCESS;
            }

            if (flags_ & flag_t::EN_TM_IN_RESET) {
                return copp::COPP_EC_IN_RESET;
            }

            // first tick, init and reset task timeout
            if (0 == last_tick_time_.tv_sec && 0 == last_tick_time_.tv_nsec) {
                // hold lock
#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
                libcopp::util::lock::lock_holder<libcopp::util::lock::spin_lock> lock_guard(action_lock_);
#endif

                std::set<detail::task_timer_node<task_t> > real_checkpoints;
                for (typename std::set<detail::task_timer_node<task_t> >::iterator iter = task_timeout_timer_.begin();
                     task_timeout_timer_.end() != iter; ++iter) {
                    detail::task_timer_node<task_t> new_checkpoint = (*iter);
                    new_checkpoint.expired_time.tv_sec += sec;
                    new_checkpoint.expired_time.tv_nsec += nsec;
                    real_checkpoints.insert(new_checkpoint);
                }

                task_timeout_timer_.swap(task_timeout_timer_);
                last_tick_time_ = now_tick_time;
                return copp::COPP_EC_SUCCESS;
            }

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
            std::list<std::exception_ptr> eptrs;
#endif
            // remove timeout tasks
            while (false == task_timeout_timer_.empty()) {
                task_ptr_t task_inst;

                {
                    // hold lock
#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
                    libcopp::util::lock::lock_holder<libcopp::util::lock::spin_lock> lock_guard(action_lock_);
#endif

                    const typename std::set<detail::task_timer_node<task_t> >::value_type &timer_node = *task_timeout_timer_.begin();
                    // all tasks those expired time less than now are timeout
                    if (now_tick_time <= timer_node.expired_time) {
                        break;
                    }

                    // check expire time(may be changed)
                    typedef typename container_t::iterator iter_type;

                    iter_type iter = tasks_.find(timer_node.task_id);

                    if (tasks_.end() != iter) {
                        // task may be removed before
                        task_inst = std::move(iter->second.task_);

                        remove_timeout_timer(iter->second);
                        tasks_.erase(iter); // remove from container
                    }
                }

                // task call can not be used when lock is on
                if (task_inst && !task_inst->is_exiting()) {
#if defined(LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER) && LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER
                    // already cleanup, there is no need to cleanup again
                    task_t::task_manager_helper::cleanup_task_manager(*task_inst, reinterpret_cast<void *>(this));
#endif
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
                    task_inst->kill(eptrs, EN_TS_TIMEOUT, nullptr);
#else
                    task_inst->kill(EN_TS_TIMEOUT);
#endif
                }
            }

            last_tick_time_ = now_tick_time;

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
            task_t::maybe_rethrow(eptrs);
#endif
            return copp::COPP_EC_SUCCESS;
        }

        /**
         * @brief get timeout checkpoint number in this manager
         * @return checkpoint number
         */
        size_t get_tick_checkpoint_size() const LIBCOPP_MACRO_NOEXCEPT { return task_timeout_timer_.size(); }

        /**
         * @brief get task number in this manager
         * @return task number
         */
        size_t get_task_size() const LIBCOPP_MACRO_NOEXCEPT { return tasks_.size(); }

        /**
         * @brief get last tick time
         * @return last tick time
         */
        detail::tickspec_t get_last_tick_time() const LIBCOPP_MACRO_NOEXCEPT { return last_tick_time_; }

        /**
         * @brief task container, this api is just used for provide information to users
         * @return task container
         */
        inline const container_t &get_container() const LIBCOPP_MACRO_NOEXCEPT { return tasks_; }

        /**
         * @brief get all task checkpoints, this api is just used for provide information to users
         * @return task checkpoints
         */
        inline const std::set<detail::task_timer_node<task_t> > &get_checkpoints() const LIBCOPP_MACRO_NOEXCEPT {
            return task_timeout_timer_;
        }

    private:
        void set_timeout_timer(detail::task_manager_node<task_t> &node, time_t timeout_sec, int timeout_nsec) {
            remove_timeout_timer(node);

            if (timeout_sec <= 0 && timeout_nsec <= 0) {
                return;
            }

            if (!node.task_) {
                return;
            }

            detail::task_timer_node<task_t> timer_node;
            timer_node.task_id              = node.task_->get_id();
            timer_node.expired_time.tv_sec  = last_tick_time_.tv_sec + timeout_sec;
            timer_node.expired_time.tv_nsec = last_tick_time_.tv_nsec + timeout_nsec;

            std::pair<typename std::set<detail::task_timer_node<task_t> >::iterator, bool> res = task_timeout_timer_.insert(timer_node);
            if (res.second) {
                node.timer_node = res.first;
            }
        }

        void remove_timeout_timer(detail::task_manager_node<task_t> &node) {
            if (node.timer_node != task_timeout_timer_.end()) {
                task_timeout_timer_.erase(node.timer_node);
                node.timer_node = task_timeout_timer_.end();
            }
        }

#if defined(LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER) && LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER
        static void task_cleanup_callback(void *self_ptr, task_t &task_inst) {
            if (UTIL_CONFIG_NULLPTR == self_ptr) {
                return;
            }

            reinterpret_cast<self_t *>(self_ptr)->remove_task(task_inst.get_id(), &task_inst);
        }
#endif

    private:
        container_t                                tasks_;
        detail::tickspec_t                         last_tick_time_;
        std::set<detail::task_timer_node<task_t> > task_timeout_timer_;

#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
        libcopp::util::lock::spin_lock action_lock_;
#endif
        int flags_;
    };
} // namespace cotask


#endif /* TASK_MANAGER_H_ */
