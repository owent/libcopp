
#include <libcotask/impl/task_action_impl.h>

namespace cotask {
namespace impl {
LIBCOPP_COTASK_API task_action_impl::task_action_impl() {}

LIBCOPP_COTASK_API task_action_impl::~task_action_impl() {}

task_action_impl::task_action_impl(const task_action_impl &) {}
task_action_impl &task_action_impl::operator=(const task_action_impl &) { return *this; }

task_action_impl::task_action_impl(const task_action_impl &&) {}
task_action_impl &task_action_impl::operator=(const task_action_impl &&) { return *this; }

}  // namespace impl
}  // namespace cotask
