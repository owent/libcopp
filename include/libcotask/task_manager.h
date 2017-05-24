/*
 * task_manager.h
 *
 *  Created on: 2014年6月16日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef _COTASK_TASK_MANAGER_H_
#define _COTASK_TASK_MANAGER_H_

#include <algorithm>
#include <assert.h>
#include <ctime>
#include <map>
#include <stdint.h>
#include <vector>

#include <libcotask/task_macros.h>


namespace cotask {

    namespace detail {
        struct tickspec_t {
            time_t tv_sec; /* Seconds.  */
            int tv_nsec;   /* Nanoseconds.  */

            friend bool operator<(const tickspec_t &l, const tickspec_t &r) {
                return (l.tv_sec != r.tv_sec) ? l.tv_sec < r.tv_sec : l.tv_nsec < r.tv_nsec;
            }

            friend bool operator==(const tickspec_t &l, const tickspec_t &r) { return l.tv_sec == r.tv_sec && l.tv_nsec == r.tv_nsec; }

            friend bool operator<=(const tickspec_t &l, const tickspec_t &r) {
                return (l.tv_sec != r.tv_sec) ? l.tv_sec <= r.tv_sec : l.tv_nsec <= r.tv_nsec;
            }
        };
    }

    template <typename TID>
    struct task_mgr_node {
        detail::tickspec_t expired_time_;
        impl::task_impl::ptr_t task_;
    };

    /**
     * @brief task manager
     */
    template <typename TID = macro_task::id_t, typename TTaskContainer = std::map<TID, task_mgr_node<TID> > >
    class task_manager {
    public:
        typedef TID id_t;
        typedef TTaskContainer container_t;
        typedef impl::task_impl::ptr_t task_ptr_t;
        typedef task_manager<id_t, container_t> self_t;
        typedef std::shared_ptr<self_t> ptr_t;

        struct flag_t {
            enum type {
                EN_TM_NONE = 0x00,
                EN_TM_IN_TICK = 0x01,
                EN_TM_IN_RESET = 0x02,
            };
        };

    private:
        struct flag_guard_t {
            int *data_;
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
            last_tick_time_.tv_sec = 0;
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

            std::vector<impl::task_impl::ptr_t> all_tasks;
            // first, lock and reset all data
            {
#if !defined(PROJECT_DISABLE_MT) || !(PROJECT_DISABLE_MT)
                util::lock::lock_holder<util::lock::spin_lock> lock_guard(action_lock_);
#endif

                for (typename container_t::iterator iter = tasks_.begin(); iter != tasks_.end(); ++iter) {
                    all_tasks.push_back(iter->second.task_);
                }

                tasks_.clear();
                task_timeout_checkpoints_.clear();
                flags_ = 0;
                last_tick_time_.tv_sec = 0;
                last_tick_time_.tv_nsec = 0;
            }

            // then, kill all tasks
            for (typename std::vector<impl::task_impl::ptr_t>::iterator iter = all_tasks.begin(); iter != all_tasks.end(); ++iter) {
                (*iter)->kill(EN_TS_KILLED);
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
         *        please make sure the task inherited from task_impl
         *
         * @param task task to be inserted
         * @param timeout_sec timeout in second ( unix time stamp recommanded )
         * @param timeout_nsec timeout in nanosecond ( must be in the range 0-999999999 )
         * @return 0 or error code
         *
         * @note if a task added before the first calling of tick method,
         *       the timeout will be set releative to the first calling time of tick method
         * @see impl::task_impl
         * @see tick
         */
        template <typename TTask>
        int add_task(std::shared_ptr<TTask> task, time_t timeout_sec, int timeout_nsec) {
            if (!task) {
                assert(task);
                return copp::COPP_EC_ARGS_ERROR;
            }

            if (flags_ & flag_t::EN_TM_IN_RESET) {
                return copp::COPP_EC_IN_RESET;
            }

            // try to cast type
            typedef typename container_t::value_type pair_type;
            task_mgr_node<id_t> task_node;
            task_node.task_ = std::dynamic_pointer_cast<impl::task_impl>(task);
            task_node.expired_time_.tv_sec = last_tick_time_.tv_sec + timeout_sec;
            task_node.expired_time_.tv_nsec = last_tick_time_.tv_nsec + timeout_nsec;

            if (!task_node.task_) {
                assert(task_node.task_ && "cast to cotask::impl::task_impl failed.");
                return copp::COPP_EC_CAST_FAILED;
            }

            // lock before we will operator tasks_
#if !defined(PROJECT_DISABLE_MT) || !(PROJECT_DISABLE_MT)
            util::lock::lock_holder<util::lock::spin_lock> lock_guard(action_lock_);
#endif

            id_t task_id = task->get_id();
            if (tasks_.end() != tasks_.find(task_id)) {
                return copp::COPP_EC_ALREADY_EXIST;
            }

            // try to insert to container
            if (false == tasks_.insert(pair_type(task_id, task_node)).second) {
                return copp::COPP_EC_EXTERNAL_INSERT_FAILED;
            }

            // add timeout controller
            if (0 != timeout_sec || 0 != timeout_nsec) {
                typedef typename std::multimap<detail::tickspec_t, id_t>::value_type pair_type;
                task_timeout_checkpoints_.insert(pair_type(task_node.expired_time_, task_id));
            }

            return copp::COPP_EC_SUCCESS;
        }

        /**
         * @brief add task to manager
         *        please make the task has method of get_id() and will return a unique id
         *        please make sure the task inherited from task_impl
         *
         * @param task task to be inserted
         * @return 0 or error code
         *
         * @see impl::task_impl
         */
        template <typename TTask>
        int add_task(std::shared_ptr<TTask> task) {
            return add_task<TTask>(task, 0, 0);
        }

        /**
         * @brief remove task in this manager
         * @param id task id
         * @return 0 or error code
         */
        int remove_task(id_t id) {
            if (flags_ & flag_t::EN_TM_IN_RESET) {
                return copp::COPP_EC_IN_RESET;
            }

            impl::task_impl::ptr_t task_inst;
            {
#if !defined(PROJECT_DISABLE_MT) || !(PROJECT_DISABLE_MT)
                util::lock::lock_holder<util::lock::spin_lock> lock_guard(action_lock_);
#endif

                typedef typename container_t::iterator iter_type;
                iter_type iter = tasks_.find(id);
                if (tasks_.end() == iter) return copp::COPP_EC_NOT_FOUND;

                // make sure running task be killed first
                task_inst = iter->second.task_;
                tasks_.erase(iter);
            }

            if (task_inst) {
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

#if !defined(PROJECT_DISABLE_MT) || !(PROJECT_DISABLE_MT)
            util::lock::lock_holder<util::lock::spin_lock> lock_guard(action_lock_);
#endif

            typedef typename container_t::iterator iter_type;
            iter_type iter = tasks_.find(id);
            if (tasks_.end() == iter) return task_ptr_t();

            return iter->second.task_;
        }

        // int add_scheduler();
        // int scheduling_once();
        // int scheduling_loop();

        int start(id_t id, void *priv_data = NULL) {
            if (flags_ & flag_t::EN_TM_IN_RESET) {
                return copp::COPP_EC_IN_RESET;
            }

            impl::task_impl::ptr_t task_inst;
            {
#if !defined(PROJECT_DISABLE_MT) || !(PROJECT_DISABLE_MT)
                util::lock::lock_holder<util::lock::spin_lock> lock_guard(action_lock_);
#endif

                typedef typename container_t::iterator iter_type;
                iter_type iter = tasks_.find(id);
                if (tasks_.end() == iter) return copp::COPP_EC_NOT_FOUND;

                task_inst = iter->second.task_;
            }

            // unlock and then run start
            if (task_inst) {
                int ret = task_inst->start(priv_data);

                // if task is finished, remove it
                if (task_inst->get_status() >= EN_TS_DONE) {
                    // lock again and prepare to remove from tasks_
#if !defined(PROJECT_DISABLE_MT) || !(PROJECT_DISABLE_MT)
                    util::lock::lock_holder<util::lock::spin_lock> lock_guard(action_lock_);
#endif
                    tasks_.erase(id);
                }

                return ret;
            } else {
                return copp::COPP_EC_NOT_FOUND;
            }
        }

        int resume(id_t id, void *priv_data = NULL) {
            if (flags_ & flag_t::EN_TM_IN_RESET) {
                return copp::COPP_EC_IN_RESET;
            }

            impl::task_impl::ptr_t task_inst;
            {
#if !defined(PROJECT_DISABLE_MT) || !(PROJECT_DISABLE_MT)
                util::lock::lock_holder<util::lock::spin_lock> lock_guard(action_lock_);
#endif

                typedef typename container_t::iterator iter_type;
                iter_type iter = tasks_.find(id);
                if (tasks_.end() == iter) return copp::COPP_EC_NOT_FOUND;

                task_inst = iter->second.task_;
            }

            // unlock and then run resume
            if (task_inst) {
                int ret = task_inst->resume(priv_data);

                // if task is finished, remove it
                if (task_inst->get_status() >= EN_TS_DONE) {
                    // lock again and prepare to remove from tasks_
#if !defined(PROJECT_DISABLE_MT) || !(PROJECT_DISABLE_MT)
                    util::lock::lock_holder<util::lock::spin_lock> lock_guard(action_lock_);
#endif
                    tasks_.erase(id);
                }

                return ret;
            } else {
                return copp::COPP_EC_NOT_FOUND;
            }
        }

        int cancel(id_t id, void *priv_data = NULL) {
            if (flags_ & flag_t::EN_TM_IN_RESET) {
                return copp::COPP_EC_IN_RESET;
            }

            impl::task_impl::ptr_t task_inst;
            {
#if !defined(PROJECT_DISABLE_MT) || !(PROJECT_DISABLE_MT)
                util::lock::lock_holder<util::lock::spin_lock> lock_guard(action_lock_);
#endif

                typedef typename container_t::iterator iter_type;
                iter_type iter = tasks_.find(id);
                if (tasks_.end() == iter) {
                    return copp::COPP_EC_NOT_FOUND;
                }

                task_inst = iter->second.task_;
                tasks_.erase(iter); // remove from container
            }

            // unlock and then run cancel
            if (task_inst) {
                return task_inst->cancel(priv_data);
            } else {
                return copp::COPP_EC_NOT_FOUND;
            }
        }

        int kill(id_t id, enum EN_TASK_STATUS status, void *priv_data = NULL) {
            if (flags_ & flag_t::EN_TM_IN_RESET) {
                return copp::COPP_EC_IN_RESET;
            }

            impl::task_impl::ptr_t task_inst;
            {
#if !defined(PROJECT_DISABLE_MT) || !(PROJECT_DISABLE_MT)
                util::lock::lock_holder<util::lock::spin_lock> lock_guard(action_lock_);
#endif

                typedef typename container_t::iterator iter_type;
                iter_type iter = tasks_.find(id);
                if (tasks_.end() == iter) {
                    return copp::COPP_EC_NOT_FOUND;
                }

                task_inst = iter->second.task_;
                tasks_.erase(iter); // remove from container
            }

            // unlock and then run kill
            if (task_inst) {
                return task_inst->kill(status, priv_data);
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
            now_tick_time.tv_sec = sec;
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
#if !defined(PROJECT_DISABLE_MT) || !(PROJECT_DISABLE_MT)
                util::lock::lock_holder<util::lock::spin_lock> lock_guard(action_lock_);
#endif

                std::multimap<detail::tickspec_t, id_t> real_checkpoints;
                for (typename std::multimap<detail::tickspec_t, id_t>::iterator iter = task_timeout_checkpoints_.begin();
                     task_timeout_checkpoints_.end() != iter; ++iter) {
                    detail::tickspec_t new_checkpoint;
                    new_checkpoint.tv_sec = iter->first.tv_sec + sec;
                    new_checkpoint.tv_nsec = iter->first.tv_nsec + nsec;
                    typedef typename std::multimap<detail::tickspec_t, id_t>::value_type pair_type;
                    real_checkpoints.insert(pair_type(new_checkpoint, iter->second));
                }

                real_checkpoints.swap(task_timeout_checkpoints_);
                last_tick_time_ = now_tick_time;
                return copp::COPP_EC_SUCCESS;
            }

            // remove timeout tasks
            while (false == task_timeout_checkpoints_.empty()) {
                impl::task_impl::ptr_t task_inst;

                {
                    // hold lock
#if !defined(PROJECT_DISABLE_MT) || !(PROJECT_DISABLE_MT)
                    util::lock::lock_holder<util::lock::spin_lock> lock_guard(action_lock_);
#endif

                    typename std::multimap<detail::tickspec_t, id_t>::value_type &task_node = *task_timeout_checkpoints_.begin();
                    // all tasks those expired time less than now are timeout
                    if (now_tick_time <= task_node.first) {
                        break;
                    }

                    // check expire time(may be changed)
                    typedef typename container_t::iterator iter_type;
                    iter_type iter = tasks_.find(task_node.second);
                    if (tasks_.end() != iter && iter->second.expired_time_ < now_tick_time) {
                        // task may be removed before
                        task_inst = iter->second.task_;
                        tasks_.erase(iter); // remove from container
                    }

                    // remove timeout checkpoint
                    task_timeout_checkpoints_.erase(task_timeout_checkpoints_.begin());
                }

                // task call can not be used when lock is on
                if (task_inst) {
                    task_inst->kill(EN_TS_TIMEOUT);
                }
            }

            last_tick_time_ = now_tick_time;
            return copp::COPP_EC_SUCCESS;
        }

        /**
         * @brief get timeout checkpoint number in this manager
         * @return checkpoint number
         */
        size_t get_tick_checkpoint_size() const { return task_timeout_checkpoints_.size(); }

        /**
         * @brief get task number in this manager
         * @return task number
         */
        size_t get_task_size() const { return tasks_.size(); }

        /**
         * @brief get last tick time
         * @return last tick time
         */
        detail::tickspec_t get_last_tick_time() const { return last_tick_time_; }

    private:
        container_t tasks_;
        detail::tickspec_t last_tick_time_;
        std::multimap<detail::tickspec_t, id_t> task_timeout_checkpoints_;

#if !defined(PROJECT_DISABLE_MT) || !(PROJECT_DISABLE_MT)
        util::lock::spin_lock action_lock_;
#endif
        int flags_;
    };
}


#endif /* TASK_MANAGER_H_ */
