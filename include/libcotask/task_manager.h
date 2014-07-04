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

#include <assert.h>
#include <stdint.h>
#include <ctime>
#include <map>
#include <algorithm>

#include <libcotask/task_macros.h>

namespace cotask {

    namespace detail {
        struct tickspec_t {
           time_t tv_sec;       /* Seconds.  */
           int tv_nsec;         /* Nanoseconds.  */

           friend bool operator < (const tickspec_t& l, const tickspec_t& r) {
               return (l.tv_sec != r.tv_sec)?
                   l.tv_sec < r.tv_sec :
                   l.tv_nsec < r.tv_nsec;
           }

           friend bool operator == (const tickspec_t& l, const tickspec_t& r) {
               return l.tv_sec == r.tv_sec && l.tv_nsec == r.tv_nsec;
           }

           friend bool operator <= (const tickspec_t& l, const tickspec_t& r) {
               return (l.tv_sec != r.tv_sec)?
                   l.tv_sec <= r.tv_sec :
                   l.tv_nsec <= r.tv_nsec;
           }
        };
    }

    template<typename TID>
    struct task_mgr_node {
        detail::tickspec_t expired_time_;
        impl::task_impl::ptr_t task_;
    };

    /**
     * @brief task manager
     */
    template<typename TID = macro_task::id_t, typename TTaskComtainer = std::map<TID, task_mgr_node<TID> > >
    class task_manager {
    public:
        typedef TID id_t;
        typedef TTaskComtainer container_t;
        typedef impl::task_impl::ptr_t task_ptr_t;
        typedef task_manager<id_t, container_t> self_t;
        typedef std::shared_ptr<self_t> ptr_t;


    public:
        task_manager() {
            last_tick_time_.tv_sec = 0;
            last_tick_time_.tv_nsec = 0;
        }

        ~task_manager() {}

        /**
         * @brief create a new task manager
         * @return smart pointer of task manager
         */
        static ptr_t create() {
            return ptr_t(new self_t());
        }

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
        template<typename TTask>
        int add_task(std::shared_ptr<TTask> task, time_t timeout_sec, int timeout_nsec) {
            if (!task) {
                assert(task);
                return copp::COPP_EC_ARGS_ERROR;
            }

            id_t task_id = task->get_id();
            if (tasks_.end() != tasks_.find(task_id)) {
                return copp::COPP_EC_ALREADY_EXIST;
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

            // try to insert to container
            if (false == tasks_.insert(pair_type(task_id, task_node)).second) {
                return copp::COPP_EC_EXTERNAL_INSERT_FAILED;
            }

            // add timeout controller
            if ( 0 != timeout_sec || 0 != timeout_nsec) {
                typedef typename std::multimap<detail::tickspec_t, id_t >::value_type pair_type;
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
        template<typename TTask>
        int add_task(std::shared_ptr<TTask> task) {
            return add_task<TTask>(task, 0, 0);
        }

        /**
         * @brief remove task in this manager
         * @param id task id
         * @return 0 or error code
         */
        int remove_task(id_t id) {
            typedef typename container_t::iterator iter_type;
            iter_type iter = tasks_.find(id);
            if (tasks_.end() == iter)
                return copp::COPP_EC_NOT_FOUND;

            tasks_.erase(iter);
            return copp::COPP_EC_SUCCESS;
        }

        /**
         * @brief find task by id
         * @param id task id
         * @return smart pointer of task
         */
        task_ptr_t find_task(id_t id) {
            typedef typename container_t::iterator iter_type;
            iter_type iter = tasks_.find(id);
            if (tasks_.end() == iter)
                return task_ptr_t();

            return iter->second.task_;
        }

        //int add_scheduler();
        //int scheduling_once();
        //int scheduling_loop();

        int start(id_t id) {
            typedef typename container_t::iterator iter_type;
            iter_type iter = tasks_.find(id);
            if (tasks_.end() == iter)
                return copp::COPP_EC_NOT_FOUND;

            int ret = iter->second.task_->start();
            if (iter->second.task_->get_status() >= EN_TS_DONE)
                tasks_.erase(iter);

            return ret;
        }

        int resume(id_t id) {
            typedef typename container_t::iterator iter_type;
            iter_type iter = tasks_.find(id);
            if (tasks_.end() == iter)
                return copp::COPP_EC_NOT_FOUND;

            int ret = iter->second.task_->resume();
            if (iter->second.task_->get_status() >= EN_TS_DONE)
                tasks_.erase(iter);

            return ret;
        }

        int cancel(id_t id) {
            task_ptr_t task = find_task(id);
            if(!task)
                return copp::COPP_EC_NOT_FOUND;

            int ret = task->cancel();
            remove_task(id); // remove from container
            return ret;
        }

        int kill(id_t id, enum EN_TASK_STATUS status) {
            task_ptr_t task = find_task(id);
            if(!task)
                return copp::COPP_EC_NOT_FOUND;

            int ret = task->kill(status);
            remove_task(id); // remove from container
            return ret;
        }

        int kill(id_t id) {
            return kill(id, EN_TS_KILLED);
        }

        /**
         * @brief active tick event and deal with clock
         * @param sec current time in second ( unix time stamp recommanded )
         * @param nsec current time in nanosecond ( must be in the range 0-999999999 )
         * @return 0 or error code
         *
         * @note tasks timeout and removed here
         */
        int tick(time_t sec, int nsec = 0) {
            detail::tickspec_t now_tick_time;
            now_tick_time.tv_sec = sec;
            now_tick_time.tv_nsec = nsec;

            // first tick, init and reset task timeout
            if (0 == last_tick_time_.tv_sec && 0 == last_tick_time_.tv_nsec) {
                std::multimap<detail::tickspec_t, id_t > real_checkpoints;
                for(typename std::multimap<detail::tickspec_t, id_t >::iterator iter = task_timeout_checkpoints_.begin();
                    task_timeout_checkpoints_.end() != iter;
                    ++ iter) {
                    detail::tickspec_t new_checkpoint;
                    new_checkpoint.tv_sec = iter->first.tv_sec + sec;
                    new_checkpoint.tv_nsec = iter->first.tv_nsec + nsec;
                    typedef typename std::multimap<detail::tickspec_t, id_t >::value_type pair_type;
                    real_checkpoints.insert(pair_type(new_checkpoint, iter->second));
                }

                real_checkpoints.swap(task_timeout_checkpoints_);
                last_tick_time_ = now_tick_time;
                return copp::COPP_EC_SUCCESS;
            }

            // remove timeout tasks
            while(false == task_timeout_checkpoints_.empty()) {
                typename std::multimap<detail::tickspec_t, id_t >::value_type& task_node = *task_timeout_checkpoints_.begin();
                // all tasks those expired time less than now are timeout
                if (now_tick_time <= task_node.first) {
                    break;
                }

                // check expire time(may be changed)
                typedef typename container_t::iterator iter_type;
                iter_type iter = tasks_.find(task_node.second);
                if (tasks_.end() != iter && iter->second.expired_time_ < now_tick_time) {
                    // task may be removed before
                    iter->second.task_->kill(EN_TS_TIMEOUT);
                    tasks_.erase(iter); // remove from container
                }

                // remove timeout checkpoint
                task_timeout_checkpoints_.erase(task_timeout_checkpoints_.begin());
            }

            last_tick_time_ = now_tick_time;
            return copp::COPP_EC_SUCCESS;
        }

        /**
         * @brief get timeout checkpoint number in this manager
         * @return checkpoint number
         */
        size_t get_tick_checkpoint_size() {
            return task_timeout_checkpoints_.size();
        }

        /**
         * @brief get task number in this manager
         * @return task number
         */
        size_t get_task_size() {
            return tasks_.size();
        }

        /**
         * @brief get last tick time
         * @return last tick time
         */
        detail::tickspec_t get_last_tick_time() {
            return last_tick_time_;
        }
    private:
        container_t tasks_;
        detail::tickspec_t last_tick_time_;
        std::multimap<detail::tickspec_t, id_t > task_timeout_checkpoints_;
    };
}



#endif /* TASK_MANAGER_H_ */
