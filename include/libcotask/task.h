// Copyright 2023 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/coroutine/std_coroutine_common.h>
#include <libcopp/future/future.h>
#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/stack/stack_traits.h>
#include <libcopp/utils/errno.h>
#include <libcotask/task_macros.h>
#include <libcotask/this_task.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
#  include <exception>
#endif

#if defined(LIBCOPP_MACRO_ENABLE_WIN_FIBER) && LIBCOPP_MACRO_ENABLE_WIN_FIBER
#  include <type_traits>
#endif
#include <stdint.h>
#include <algorithm>
#include <cstddef>
#include <list>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

LIBCOPP_COTASK_NAMESPACE_BEGIN

template <typename TCO_MACRO = macro_coroutine>
class LIBCOPP_COTASK_API_HEAD_ONLY task : public impl::task_impl {
 public:
  using macro_coroutine_type = TCO_MACRO;
  using self_type = task<macro_coroutine_type>;
  using ptr_type = LIBCOPP_COPP_NAMESPACE_ID::util::intrusive_ptr<self_type>;

  using coroutine_type = typename macro_coroutine_type::coroutine_type;
  using stack_allocator_type = typename macro_coroutine_type::stack_allocator_type;

  using id_type = typename impl::task_impl::id_type;
  using id_allocator_type = typename impl::task_impl::id_allocator_type;

  // Compability with libcopp-1.x
  using macro_coroutine_t = macro_coroutine_type;
  using self_t = self_type;
  using ptr_t = ptr_type;
  using coroutine_t = coroutine_type;
  using stack_allocator_t = stack_allocator_type;
  using id_allocator_t = id_allocator_type;
  using id_t = id_type;

 private:
  using action_ptr_type = impl::task_impl::action_ptr_type;
  // Compability with libcopp-1.x
  using action_ptr_t = action_ptr_type;

  struct task_group {
    std::list<std::pair<ptr_type, void *> > member_list_;
  };

 public:
  /**
   * @brief constuctor
   * @note should not be called directly
   */
  task(size_t stack_sz)
      : stack_size_(stack_sz),
        action_destroy_fn_(nullptr)
#if defined(LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER) && LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER
        ,
        binding_manager_ptr_(nullptr),
        binding_manager_fn_(nullptr)
#endif
  {
  }

  /**
   * @brief create task with functor
   * @param action
   * @param stack_size stack size
   * @param private_buffer_size buffer size to store private data
   * @return task smart pointer
   */
  template <typename TAct, typename Ty>
  static LIBCOPP_COTASK_API_HEAD_ONLY ptr_type create_with_delegate(Ty &&callable,
                                                                    typename coroutine_type::allocator_type &alloc,
                                                                    size_t stack_size = 0,
                                                                    size_t private_buffer_size = 0) {
    using a_t = TAct;

    if (0 == stack_size) {
      stack_size = LIBCOPP_COPP_NAMESPACE_ID::stack_traits::default_size();
    }

    size_t action_size = coroutine_type::align_address_size(sizeof(a_t));
    size_t task_size = coroutine_type::align_address_size(sizeof(self_type));

    if (stack_size <= sizeof(impl::task_impl *) + private_buffer_size + action_size + task_size) {
      return ptr_type();
    }

    typename coroutine_type::ptr_type coroutine =
        coroutine_type::create(typename coroutine_type::callback_t(), alloc, stack_size,
                               sizeof(impl::task_impl *) + private_buffer_size, action_size + task_size);
    if (!coroutine) {
      return ptr_type();
    }

    void *action_addr = sub_buffer_offset(coroutine.get(), action_size);
    void *task_addr = sub_buffer_offset(action_addr, task_size);

    // placement new task
    ptr_type ret(new (task_addr) self_type(stack_size));
    if (!ret) {
      return ret;
    }

    *(reinterpret_cast<impl::task_impl **>(coroutine->get_private_buffer())) = ret.get();
    ret->coroutine_obj_ = coroutine;
    ret->coroutine_obj_->set_flags(impl::task_impl::ext_coroutine_flag_t::EN_ECFT_COTASK);

    // placement new action
    a_t *action = new (action_addr) a_t(COPP_MACRO_STD_FORWARD(Ty, callable));
    if (nullptr == action) {
      return ret;
    }

    // redirect runner
    coroutine->set_runner([action](void *private_data) { return (*action)(private_data); });

    ret->action_destroy_fn_ = get_placement_destroy(action);
    ret->_set_action(action);

    return ret;
  }

  /**
   * @brief create task with functor
   * @param action
   * @param stack_size stack size
   * @param private_buffer_size buffer size to store private data
   * @return task smart pointer
   */
  template <typename Ty>
  static inline ptr_type create(Ty &&functor, size_t stack_size = 0, size_t private_buffer_size = 0) {
    typename coroutine_type::allocator_type alloc;
    return create(std::forward<Ty>(functor), alloc, stack_size, private_buffer_size);
  }

  template <typename Ty>
  static inline ptr_type create(Ty &&functor, typename coroutine_type::allocator_type &alloc, size_t stack_size = 0,
                                size_t private_buffer_size = 0) {
    using decay_type = typename std::decay<Ty>::type;
    using a_t = typename std::conditional<std::is_base_of<impl::task_action_impl, decay_type>::value, decay_type,
                                          task_action_functor<decay_type> >::type;

    return create_with_delegate<a_t>(std::forward<Ty>(functor), alloc, stack_size, private_buffer_size);
  }

  /**
   * @brief create task with function
   * @param action
   * @param stack_size stack size
   * @return task smart pointer
   */
  template <typename Ty>
  static inline ptr_type create(Ty (*func)(void *), typename coroutine_type::allocator_type &alloc,
                                size_t stack_size = 0, size_t private_buffer_size = 0) {
    using a_t = task_action_function<Ty>;

    return create_with_delegate<a_t>(func, alloc, stack_size, private_buffer_size);
  }

  template <typename Ty>
  static inline ptr_type create(Ty (*func)(void *), size_t stack_size = 0, size_t private_buffer_size = 0) {
    typename coroutine_type::allocator_type alloc;
    return create(func, alloc, stack_size, private_buffer_size);
  }

  /**
   * @brief create task with function
   * @param action
   * @param stack_size stack size
   * @return task smart pointer
   */
  template <typename Ty, typename TInst>
  static LIBCOPP_COTASK_API_HEAD_ONLY ptr_type create(Ty(TInst::*func), TInst *instance,
                                                      typename coroutine_type::allocator_type &alloc,
                                                      size_t stack_size = 0, size_t private_buffer_size = 0) {
    using a_t = task_action_mem_function<Ty, TInst>;

    return create<a_t>(a_t(func, instance), alloc, stack_size, private_buffer_size);
  }

  template <typename Ty, typename TInst>
  static inline ptr_type create(Ty(TInst::*func), TInst *instance, size_t stack_size = 0,
                                size_t private_buffer_size = 0) {
    typename coroutine_type::allocator_type alloc;
    return create(func, instance, alloc, stack_size, private_buffer_size);
  }

  /**
   * @brief create task with functor type and parameters
   * @param stack_size stack size
   * @param args all parameters passed to construtor of type Ty
   * @return task smart pointer
   */
  template <typename Ty, typename... TParams>
  static LIBCOPP_COTASK_API_HEAD_ONLY ptr_type create_with(typename coroutine_type::allocator_type &alloc,
                                                           size_t stack_size, size_t private_buffer_size,
                                                           TParams &&...args) {
    using a_t = Ty;

    return create(a_t(std::forward<TParams>(args)...), alloc, stack_size, private_buffer_size);
  }

  /**
   * @brief add next task to run when task finished
   * @note please not to make tasks refer to each other. [it will lead to memory leak]
   * @note [don't do that] ptr_type a = ..., b = ...; a.next(b); b.next(a);
   * @param next_task next stack
   * @param priv_data priv_data passed to resume or start next stack
   * @return next_task if success , or self if failed
   */
  inline ptr_type next(ptr_type next_task, void *priv_data = nullptr) {
    // can not refers to self
    if (this == next_task.get() || !next_task) {
      return ptr_type(this);
    }

    // can not add next task when finished
    if (is_exiting() || is_completed()) {
      // run next task immedialy
      EN_TASK_STATUS next_task_status = next_task->get_status();
      if (EN_TS_CREATED == next_task_status) {
        next_task->start(priv_data);
      } else if (EN_TS_WAITING == next_task_status) {
        next_task->resume(priv_data);
      }
      return next_task;
    }

#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
    LIBCOPP_COPP_NAMESPACE_ID::util::lock::lock_holder<LIBCOPP_COPP_NAMESPACE_ID::util::lock::spin_lock> lock_guard(
        inner_action_lock_);
#endif

    next_list_.member_list_.push_back(std::make_pair(next_task, priv_data));
    return next_task;
  }

  /**
   * @brief create next task with functor
   * @see next
   * @param functor
   * @param priv_data priv_data passed to start functor
   * @param stack_size stack size
   * @return the created task if success , or self if failed
   */
  template <typename Ty>
  inline ptr_type next(Ty &&functor, void *priv_data = nullptr, size_t stack_size = 0, size_t private_buffer_size = 0) {
    return next(create(std::forward<Ty>(functor), stack_size, private_buffer_size), priv_data);
  }

  template <typename Ty>
  inline ptr_type next(Ty &&functor, typename coroutine_type::allocator_type &alloc, void *priv_data = nullptr,
                       size_t stack_size = 0, size_t private_buffer_size = 0) {
    return next(create(std::forward<Ty>(functor), alloc, stack_size, private_buffer_size), priv_data);
  }

  /**
   * @brief create next task with function
   * @see next
   * @param func function
   * @param priv_data priv_data passed to start function
   * @param stack_size stack size
   * @return the created task if success , or self if failed
   */
  template <typename Ty>
  inline ptr_type next(Ty (*func)(void *), void *priv_data = nullptr, size_t stack_size = 0,
                       size_t private_buffer_size = 0) {
    return next(create(func, stack_size, private_buffer_size), priv_data);
  }

  template <typename Ty>
  inline ptr_type next(Ty (*func)(void *), typename coroutine_type::allocator_type &alloc, void *priv_data = nullptr,
                       size_t stack_size = 0, size_t private_buffer_size = 0) {
    return next(create(func, alloc, stack_size, private_buffer_size), priv_data);
  }

  /**
   * @brief create next task with function
   * @see next
   * @param func member function
   * @param instance instance
   * @param priv_data priv_data passed to start (instance->*func)(priv_data)
   * @param stack_size stack size
   * @return the created task if success , or self if failed
   */
  template <typename Ty, typename TInst>
  inline ptr_type next(Ty(TInst::*func), TInst *instance, void *priv_data = nullptr, size_t stack_size = 0,
                       size_t private_buffer_size = 0) {
    return next(create(func, instance, stack_size, private_buffer_size), priv_data);
  }

  template <typename Ty, typename TInst>
  inline ptr_type next(Ty(TInst::*func), TInst *instance, typename coroutine_type::allocator_type &alloc,
                       void *priv_data = nullptr, size_t stack_size = 0, size_t private_buffer_size = 0) {
    return next(create(func, instance, alloc, stack_size, private_buffer_size), priv_data);
  }

  /**
   * @brief await_task another cotask to finish
   * @note please not to make tasks refer to each other. [it will lead to memory leak]
   * @note [don't do that] ptr_type a = ..., b = ...; a.await_task(b); b.await_task(a);
   * @param wait_task which stack to wait for
   * @note we will loop wait and ignore any resume(...) call
   * @return 0 or error code
   */
  inline int await_task(ptr_type wait_task) {
    if (!wait_task) {
      return LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_ARGS_ERROR;
    }

    if (this == wait_task.get()) {
      return LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_TASK_CAN_NOT_WAIT_SELF;
    }

    // if target is exiting or completed, just return
    if (wait_task->is_exiting() || wait_task->is_completed()) {
      return LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_TASK_IS_EXITING;
    }

    if (is_exiting()) {
      return LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_TASK_IS_EXITING;
    }

    if (this_task() != this) {
      return LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_TASK_NOT_IN_ACTION;
    }

    // add to next list failed
    if (wait_task->next(ptr_type(this)).get() != this) {
      return LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_TASK_ADD_NEXT_FAILED;
    }

    int ret = 0;
    while (!(wait_task->is_exiting() || wait_task->is_completed())) {
      if (is_exiting()) {
        return LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_TASK_IS_EXITING;
      }

      ret = yield();
    }

    return ret;
  }

  template <typename TTask>
  inline int await_task(TTask *wait_task) {
    return await_task(ptr_type(wait_task));
  }

  /**
   * @brief add task to run when task finished
   * @note please not to make tasks refer to each other. [it will lead to memory leak]
   * @note [don't do that] ptr_type a = ..., b = ...; a.then(b); b.then(a);
   * @param next_task then stack
   * @param priv_data priv_data passed to resume or start the stack
   * @return next_task if success , or self if failed
   */
  inline ptr_type then(ptr_type next_task, void *priv_data = nullptr) { return next(next_task, priv_data); }

  /**
   * @brief create next task with functor using the same allocator and private buffer size as this task
   * @see next
   * @param functor
   * @param priv_data priv_data passed to start functor
   * @return the created task if success , or self if failed
   */
  template <typename Ty>
  inline ptr_type then(Ty &&functor, void *priv_data = nullptr) {
    if (!coroutine_obj_) {
      then(create(std::forward<Ty>(functor), stack_size_, get_private_buffer_size()), priv_data);
    }

    return then(
        create(std::forward<Ty>(functor), coroutine_obj_->get_allocator(), stack_size_, get_private_buffer_size()),
        priv_data);
  }

  template <typename Ty>
  inline ptr_type then(Ty (*func)(void *), void *priv_data = nullptr) {
    if (!coroutine_obj_) {
      return then(create(func, stack_size_, get_private_buffer_size()), priv_data);
    }

    return then(create(func, coroutine_obj_->get_allocator(), stack_size_, get_private_buffer_size()), priv_data);
  }

  /**
   * get current running task and convert to task object
   * @return task smart pointer
   */
  static self_type *this_task() {
#if defined(LIBCOPP_MACRO_ENABLE_RTTI) && LIBCOPP_MACRO_ENABLE_RTTI
    return dynamic_cast<self_type *>(impl::task_impl::this_task());
#else
    return static_cast<self_type *>(impl::task_impl::this_task());
#endif
  }

 public:
  virtual ~task() {
    EN_TASK_STATUS status = get_status();
    // inited but not finished will trigger timeout or finish other actor
    if (status < EN_TS_DONE && status > EN_TS_CREATED) {
      kill(EN_TS_TIMEOUT);
    } else if (status <= EN_TS_CREATED) {
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
      std::list<std::exception_ptr> eptrs;
      active_next_tasks(eptrs);
      // next tasks
      maybe_rethrow(eptrs);
#else
      active_next_tasks();
#endif
    }
  }

  inline typename coroutine_type::ptr_type &get_coroutine_context() LIBCOPP_MACRO_NOEXCEPT { return coroutine_obj_; }
  inline const typename coroutine_type::ptr_type &get_coroutine_context() const LIBCOPP_MACRO_NOEXCEPT {
    return coroutine_obj_;
  }

 public:
  int get_ret_code() const override {
    if (!coroutine_obj_) {
      return 0;
    }

    return coroutine_obj_->get_ret_code();
  }

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
  int start(void *priv_data, EN_TASK_STATUS expected_status = EN_TS_CREATED) override {
    std::list<std::exception_ptr> eptrs;
    int ret = start(eptrs, priv_data, expected_status);
    maybe_rethrow(eptrs);
    return ret;
  }

  virtual int start(std::list<std::exception_ptr> &unhandled, void *priv_data,
                    EN_TASK_STATUS expected_status = EN_TS_CREATED) LIBCOPP_MACRO_NOEXCEPT {
#else
  int start(void *priv_data, EN_TASK_STATUS expected_status = EN_TS_CREATED) override {
#endif
    if (!coroutine_obj_) {
      return LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_INITED;
    }

    EN_TASK_STATUS from_status = expected_status;

    do {
      COPP_UNLIKELY_IF (from_status >= EN_TS_DONE) {
        return LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_ALREADY_FINISHED;
      }

      COPP_UNLIKELY_IF (from_status == EN_TS_RUNNING) {
        return LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_IS_RUNNING;
      }

      COPP_LIKELY_IF (_cas_status(from_status, EN_TS_RUNNING)) {  // Atomic.CAS here
        break;
      }
    } while (true);

    // use this smart ptr to avoid destroy of this
    // ptr_type protect_from_destroy(this);

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
    std::exception_ptr eptr;
    int ret = coroutine_obj_->start(eptr, priv_data);
    if (eptr) {
      unhandled.emplace_back(std::move(eptr));
    }
#else
    int ret = coroutine_obj_->start(priv_data);
#endif

    from_status = EN_TS_RUNNING;
    if (is_completed()) {  // Atomic.CAS here
      while (from_status < EN_TS_DONE) {
        COPP_LIKELY_IF (_cas_status(from_status, EN_TS_DONE)) {  // Atomic.CAS here
          break;
        }
      }

      finish_priv_data_ = priv_data;
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
      _notify_finished(unhandled, priv_data);
#else
      _notify_finished(priv_data);
#endif
      return ret;
    }

    while (true) {
      if (from_status >= EN_TS_DONE) {  // canceled or killed
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
        _notify_finished(unhandled, finish_priv_data_);
#else
        _notify_finished(finish_priv_data_);
#endif
        break;
      }

      COPP_LIKELY_IF (_cas_status(from_status, EN_TS_WAITING)) {  // Atomic.CAS here
        break;
        // waiting
      }
    }

    return ret;
  }

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
  int resume(void *priv_data, EN_TASK_STATUS expected_status = EN_TS_WAITING) override {
    return start(priv_data, expected_status);
  }

  virtual int resume(std::list<std::exception_ptr> &unhandled, void *priv_data,
                     EN_TASK_STATUS expected_status = EN_TS_WAITING) LIBCOPP_MACRO_NOEXCEPT {
    return start(unhandled, priv_data, expected_status);
  }
#else
  int resume(void *priv_data, EN_TASK_STATUS expected_status = EN_TS_WAITING) override {
    return start(priv_data, expected_status);
  }
#endif

  int yield(void **priv_data) override {
    if (!coroutine_obj_) {
      return LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_INITED;
    }

    return coroutine_obj_->yield(priv_data);
  }

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
  int cancel(void *priv_data) override {
    std::list<std::exception_ptr> eptrs;
    int ret = cancel(eptrs, priv_data);
    maybe_rethrow(eptrs);
    return ret;
  }

  virtual int cancel(std::list<std::exception_ptr> &unhandled, void *priv_data) LIBCOPP_MACRO_NOEXCEPT {
#else
  int cancel(void *priv_data) override {
#endif

    EN_TASK_STATUS from_status = get_status();

    do {
      if (EN_TS_RUNNING == from_status) {
        return LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_IS_RUNNING;
      }

      COPP_LIKELY_IF (_cas_status(from_status, EN_TS_CANCELED)) {
        break;
      }
    } while (true);

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
    _notify_finished(unhandled, priv_data);
#else
    _notify_finished(priv_data);
#endif
    return LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_SUCCESS;
  }

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
  int kill(enum EN_TASK_STATUS status, void *priv_data) override {
    std::list<std::exception_ptr> eptrs;
    int ret = kill(eptrs, status, priv_data);
    maybe_rethrow(eptrs);
    return ret;
  }

  virtual int kill(std::list<std::exception_ptr> &unhandled, enum EN_TASK_STATUS status,
                   void *priv_data) LIBCOPP_MACRO_NOEXCEPT {
#else
  int kill(enum EN_TASK_STATUS status, void *priv_data) override {
#endif
    EN_TASK_STATUS from_status = get_status();

    do {
      COPP_LIKELY_IF (_cas_status(from_status, status)) {
        break;
      }
    } while (true);

    if (EN_TS_RUNNING != from_status) {
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
      _notify_finished(unhandled, priv_data);
#else
      _notify_finished(priv_data);
#endif
    } else {
      finish_priv_data_ = priv_data;
    }

    return LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_SUCCESS;
  }

  using impl::task_impl::cancel;
  using impl::task_impl::kill;
  using impl::task_impl::resume;
  using impl::task_impl::start;
  using impl::task_impl::yield;

 public:
  bool is_completed() const LIBCOPP_MACRO_NOEXCEPT override {
    if (!coroutine_obj_) {
      return false;
    }

    return coroutine_obj_->is_finished();
  }

#if defined(LIBCOPP_MACRO_ENABLE_WIN_FIBER) && LIBCOPP_MACRO_ENABLE_WIN_FIBER
  bool is_fiber() const LIBCOPP_MACRO_NOEXCEPT override {
    return std::is_base_of<LIBCOPP_COPP_NAMESPACE_ID::coroutine_context_fiber, coroutine_type>::value;
  }
#endif

  static inline void *add_buffer_offset(void *in, size_t off) {
    return reinterpret_cast<void *>(reinterpret_cast<unsigned char *>(in) + off);
  }

  static inline void *sub_buffer_offset(void *in, size_t off) {
    return reinterpret_cast<void *>(reinterpret_cast<unsigned char *>(in) - off);
  }

  void *get_private_buffer() {
    if (!coroutine_obj_) {
      return nullptr;
    }

    return add_buffer_offset(coroutine_obj_->get_private_buffer(), sizeof(impl::task_impl *));
  }

  size_t get_private_buffer_size() {
    if (!coroutine_obj_) {
      return 0;
    }

    return coroutine_obj_->get_private_buffer_size() - sizeof(impl::task_impl *);
  }

  inline size_t use_count() const { return ref_count_.load(); }

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
  UTIL_FORCEINLINE static void maybe_rethrow(std::list<std::exception_ptr> &eptrs) {
    for (std::list<std::exception_ptr>::iterator iter = eptrs.begin(); iter != eptrs.end(); ++iter) {
      coroutine_type::maybe_rethrow(*iter);
    }
  }
#endif
 private:
  task(const task &) = delete;

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
  void active_next_tasks(std::list<std::exception_ptr> &unhandled) LIBCOPP_MACRO_NOEXCEPT {
#else
  void active_next_tasks() {
#endif
    std::list<std::pair<ptr_type, void *> > next_list;
#if defined(LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER) && LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER
    void *manager_ptr;
    void (*manager_fn)(void *, self_type &);
#endif
    // first, lock and swap container
    {
#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
      LIBCOPP_COPP_NAMESPACE_ID::util::lock::lock_holder<LIBCOPP_COPP_NAMESPACE_ID::util::lock::spin_lock> lock_guard(
          inner_action_lock_);
#endif
      next_list.swap(next_list_.member_list_);
#if defined(LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER) && LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER
      manager_ptr = binding_manager_ptr_;
      manager_fn = binding_manager_fn_;
      binding_manager_ptr_ = nullptr;
      binding_manager_fn_ = nullptr;
#endif
    }

    // then, do all the pending tasks
    for (typename std::list<std::pair<ptr_type, void *> >::iterator iter = next_list.begin(); iter != next_list.end();
         ++iter) {
      if (!iter->first || EN_TS_INVALID == iter->first->get_status()) {
        continue;
      }

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
      if (iter->first->get_status() < EN_TS_RUNNING) {
        iter->first->start(unhandled, iter->second);
      } else {
        iter->first->resume(unhandled, iter->second);
      }
#else
      if (iter->first->get_status() < EN_TS_RUNNING) {
        iter->first->start(iter->second);
      } else {
        iter->first->resume(iter->second);
      }
#endif
    }

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE
    caller_manager_.resume_callers();
#endif

    // finally, notify manager to cleanup(maybe start or resume with task's API but not task_manager's)
#if defined(LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER) && LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER
    if (nullptr != manager_ptr && nullptr != manager_fn) {
      (*manager_fn)(manager_ptr, *this);
    }
#endif
  }

  int _notify_finished(
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
      std::list<std::exception_ptr> &unhandled,
#endif
      void *priv_data) LIBCOPP_MACRO_NOEXCEPT {
    // first, make sure coroutine finished.
    if (coroutine_obj_ && false == coroutine_obj_->is_finished()) {
      // make sure this task will not be destroyed when running
      while (false == coroutine_obj_->is_finished()) {
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
        std::exception_ptr eptr;
        coroutine_obj_->resume(eptr, priv_data);
        if (eptr) {
          unhandled.emplace_back(std::move(eptr));
        }
#else
        coroutine_obj_->resume(priv_data);
#endif
      }
    }

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
    int ret = impl::task_impl::_notify_finished(unhandled, priv_data);
    // next tasks
    active_next_tasks(unhandled);
#else
    int ret = impl::task_impl::_notify_finished(priv_data);
    // next tasks
    active_next_tasks();
#endif
    return ret;
  }

  friend void intrusive_ptr_add_ref(self_type *p) {
    if (p == nullptr) {
      return;
    }

    ++p->ref_count_;
  }

  friend void intrusive_ptr_release(self_type *p) {
    if (p == nullptr) {
      return;
    }

    size_t left = --p->ref_count_;
    if (0 == left) {
      // save coroutine context first, make sure it's still available after destroy task
      using this_coroutine_type = typename task<TCO_MACRO>::coroutine_type;
      using this_coroutine_ptr_type = typename this_coroutine_type::ptr_type;
      this_coroutine_ptr_type coro = p->coroutine_obj_;

      // then, find and destroy action
      void *action_ptr = reinterpret_cast<void *>(p->_get_action());
      if (nullptr != p->action_destroy_fn_ && nullptr != action_ptr) {
        (*p->action_destroy_fn_)(action_ptr);
      }

      // then, destruct task
      p->~task();

      // at last, destroy the coroutine and maybe recycle the stack space
      coro.reset();
    }
  }

#if defined(LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER) && LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER
 public:
  class LIBCOPP_COTASK_API_HEAD_ONLY task_manager_helper {
   private:
    template <class>
    friend class LIBCOPP_COTASK_API_HEAD_ONLY task_manager;
    static bool setup_task_manager(self_type &task_inst, void *manager_ptr, void (*fn)(void *, self_type &)) {
#  if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
      LIBCOPP_COPP_NAMESPACE_ID::util::lock::lock_holder<LIBCOPP_COPP_NAMESPACE_ID::util::lock::spin_lock> lock_guard(
          task_inst.inner_action_lock_);
#  endif
      if (task_inst.binding_manager_ptr_ != nullptr) {
        return false;
      }

      task_inst.binding_manager_ptr_ = manager_ptr;
      task_inst.binding_manager_fn_ = fn;
      return true;
    }

    static bool cleanup_task_manager(self_type &task_inst, void *manager_ptr) {
#  if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
      LIBCOPP_COPP_NAMESPACE_ID::util::lock::lock_holder<LIBCOPP_COPP_NAMESPACE_ID::util::lock::spin_lock> lock_guard(
          task_inst.inner_action_lock_);
#  endif
      if (task_inst.binding_manager_ptr_ != manager_ptr) {
        return false;
      }

      task_inst.binding_manager_ptr_ = nullptr;
      task_inst.binding_manager_fn_ = nullptr;
      return true;
    }
  };
#endif

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE
 public:
  class LIBCOPP_COPP_API_HEAD_ONLY stackful_task_awaitable : public LIBCOPP_COPP_NAMESPACE_ID::awaitable_base_type {
   public:
    using value_type = typename macro_coroutine_type::value_type;

   public:
    explicit stackful_task_awaitable(self_type *waiting_task) : waiting_task_(waiting_task) {}

    inline bool await_ready() const noexcept {
      if (!waiting_task_) {
        return true;
      }

      return waiting_task_->is_exiting();
    }

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
    template <LIBCOPP_COPP_NAMESPACE_ID::DerivedPromiseBaseType TCPROMISE>
#  else
    template <class TCPROMISE, typename = std::enable_if_t<
                                   std::is_base_of<LIBCOPP_COPP_NAMESPACE_ID::promise_base_type, TCPROMISE>::value> >
#  endif
    inline void await_suspend(LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TCPROMISE> caller) noexcept {
      if (waiting_task_ && !waiting_task_->is_exiting() &&
          caller.promise().get_status() < LIBCOPP_COPP_NAMESPACE_ID::promise_status::kDone) {
        set_caller(caller);
        waiting_task_->caller_manager_.add_caller(
            LIBCOPP_COPP_NAMESPACE_ID::promise_caller_manager::handle_delegate{caller});

        // Allow kill resume to forward error information
        caller.promise().set_flag(LIBCOPP_COPP_NAMESPACE_ID::promise_flag::kInternalWaitting, true);
      } else {
        // Already done and can not suspend again
        caller.resume();
      }
    }

    inline value_type await_resume() {
      if (!waiting_task_) {
        return LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_INITED;
      }

      value_type ret;
      if (waiting_task_->is_exiting()) {
        switch (waiting_task_->get_status()) {
          case EN_TS_CANCELED:
          case EN_TS_KILLED:
          case EN_TS_TIMEOUT:
            ret = LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_TASK_IS_EXITING;
            break;
          default:
            ret = waiting_task_->get_ret_code();
            break;
        }
      } else {
        ret = LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_TASK_IS_KILLED;
      }

      // caller maybe null if the callable is already ready when co_await
      auto caller = get_caller();

      if (caller) {
        if (nullptr != caller.promise) {
          caller.promise->set_flag(LIBCOPP_COPP_NAMESPACE_ID::promise_flag::kInternalWaitting, false);
        }

        waiting_task_->caller_manager_.remove_caller(caller);
        set_caller(nullptr);
      }

      return ret;
    }

   private:
    // caller manager
    self_type *waiting_task_;
  };

  auto operator co_await() & LIBCOPP_MACRO_NOEXCEPT { return stackful_task_awaitable{this}; }
#endif
 private:
  size_t stack_size_;
  typename coroutine_type::ptr_type coroutine_obj_;
  task_group next_list_;

  // ============== action information ==============
  void (*action_destroy_fn_)(void *);

#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
  LIBCOPP_COPP_NAMESPACE_ID::util::lock::atomic_int_type<size_t> ref_count_; /** ref_count **/
  LIBCOPP_COPP_NAMESPACE_ID::util::lock::spin_lock inner_action_lock_;
#else
  LIBCOPP_COPP_NAMESPACE_ID::util::lock::atomic_int_type<
      LIBCOPP_COPP_NAMESPACE_ID::util::lock::unsafe_int_type<size_t> >
      ref_count_; /** ref_count **/
#endif

  // ============== binding to task manager ==============
#if defined(LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER) && LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER
  void *binding_manager_ptr_;
  void (*binding_manager_fn_)(void *, self_type &);
#endif

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE
  LIBCOPP_COPP_NAMESPACE_ID::promise_caller_manager caller_manager_;
#endif
};

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE
template <typename TCO_MACRO>
auto operator co_await(const LIBCOPP_COPP_NAMESPACE_ID::util::intrusive_ptr<task<TCO_MACRO> > &t)
    LIBCOPP_MACRO_NOEXCEPT {
  using awaitable = typename task<TCO_MACRO>::stackful_task_awaitable;
  return awaitable{t.get()};
}
#endif
LIBCOPP_COTASK_NAMESPACE_END
