
#include <libcotask/impl/task_action_impl.h>


namespace cotask {
    namespace impl {
        LIBCOPP_COTASK_API task_action_impl::task_action_impl() {}

        LIBCOPP_COTASK_API task_action_impl::~task_action_impl() {}

        task_action_impl::task_action_impl(const task_action_impl &) {}
        task_action_impl &task_action_impl::operator=(const task_action_impl &) { return *this; }

#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
        task_action_impl::task_action_impl(const task_action_impl &&) {}
        task_action_impl &task_action_impl::operator=(const task_action_impl &&) { return *this; }
#endif

    } // namespace impl
} // namespace cotask
