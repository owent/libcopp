/*
 * test_manager.cpp
 *
 *  Created on: 2014-03-11
 *      Author: owent
 *
 *  Released under the MIT license
 */

#include <cstring>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <vector>

#include <libcopp/utils/config/compile_optimize.h>
#include <libcopp/utils/features.h>

#include "cli/cmd_option.h"
#include "cli/cmd_option_phoenix.h"
#include "cli/shell_font.h"

#include "test_manager.h"

namespace detail {
#if !(defined(THREAD_TLS_USE_PTHREAD) && THREAD_TLS_USE_PTHREAD)

struct test_manager_tls_block_t {
  int *success_counter_ptr;
  int *failed_counter_ptr;
};

#  if (defined(__cplusplus) && __cplusplus >= 201402L) || ((defined(_MSVC_LANG) && _MSVC_LANG >= 201402L))
static_assert(std::is_trivially_copyable<test_manager_tls_block_t>::value, "test_manager_tls_block_t must be trially");
#  elif (defined(__cplusplus) && __cplusplus >= 201103L) || ((defined(_MSVC_LANG) && _MSVC_LANG >= 201103L))
static_assert(std::is_trivial<test_manager_tls_block_t>::value, "test_manager_tls_block_t must be trially");
#  else
static_assert(std::is_pod<test_manager_tls_block_t>::value, "test_manager_tls_block_t must be POD");
#  endif

static test_manager_tls_block_t g_global_counter_cache = {0, 0};
test_manager_tls_block_t *get_test_manager_tls_block() {
  static thread_local test_manager_tls_block_t ret = g_global_counter_cache;
  return &ret;
}
#else

#  include <pthread.h>
struct test_manager_tls_block_t {
  int *success_counter_ptr;
  int *failed_counter_ptr;

  test_manager_tls_block_t() : success_counter_ptr(nullptr), failed_counter_ptr(nullptr) {}
};
static pthread_once_t gt_test_manager_tls_block_once = PTHREAD_ONCE_INIT;
static pthread_key_t gt_test_manager_tls_block_key;

static void dtor_pthread_test_manager_tls_block(void *p) {
  test_manager_tls_block_t *block = reinterpret_cast<test_manager_tls_block_t *>(p);
  if (nullptr != block) {
    delete block;
  }
}

static void init_pthread_test_manager_tls_block() {
  (void)pthread_key_create(&gt_test_manager_tls_block_key, dtor_pthread_test_manager_tls_block);
}

static test_manager_tls_block_t g_global_counter_cache;
test_manager_tls_block_t *get_test_manager_tls_block() {
  (void)pthread_once(&gt_test_manager_tls_block_once, init_pthread_test_manager_tls_block);
  test_manager_tls_block_t *block =
      reinterpret_cast<test_manager_tls_block_t *>(pthread_getspecific(gt_test_manager_tls_block_key));
  if (nullptr == block) {
    block = new test_manager_tls_block_t(g_global_counter_cache);
    pthread_setspecific(gt_test_manager_tls_block_key, block);
  }
  return block;
}

struct gt_test_manager_tls_block_main_thread_dtor_t {
  test_manager_tls_block_t *block_ptr;
  gt_test_manager_tls_block_main_thread_dtor_t() {
    block_ptr = get_test_manager_tls_block();
    if (nullptr != block_ptr) {
      block_ptr->success_counter_ptr = nullptr;
      block_ptr->failed_counter_ptr = nullptr;
    }
  }

  ~gt_test_manager_tls_block_main_thread_dtor_t() {
    pthread_setspecific(gt_test_manager_tls_block_key, nullptr);
    dtor_pthread_test_manager_tls_block(reinterpret_cast<void *>(block_ptr));
  }
};
static gt_test_manager_tls_block_main_thread_dtor_t gt_test_manager_tls_block_main_thread_dtor;
#endif
struct topological_sort_object_t {
  std::string name;
  void *object;

  size_t dependency_count;
  std::list<topological_sort_object_t *> depend_by;
};
}  // namespace detail

test_manager::pick_param_str_t::pick_param_str_t(const char *in) : str_(in) {}
test_manager::pick_param_str_t::pick_param_str_t(const std::string &in) : str_(in.c_str()) {}

bool test_manager::pick_param_str_t::operator==(const pick_param_str_t &other) const {
  return strcmp(str_, other.str_) == 0;
}
#ifdef __cpp_impl_three_way_comparison
std::strong_ordering test_manager::pick_param_str_t::operator<=>(const pick_param_str_t &other) const {
  int res = strcmp(str_, other.str_);
  if (res < 0) {
    return std::strong_ordering::less;
  } else if (res > 0) {
    return std::strong_ordering::greater;
  }

  return std::strong_ordering::equal;
}
#else
bool test_manager::pick_param_str_t::operator!=(const pick_param_str_t &other) const {
  return strcmp(str_, other.str_) != 0;
}
bool test_manager::pick_param_str_t::operator>=(const pick_param_str_t &other) const {
  return strcmp(str_, other.str_) >= 0;
}
bool test_manager::pick_param_str_t::operator>(const pick_param_str_t &other) const {
  return strcmp(str_, other.str_) > 0;
}
bool test_manager::pick_param_str_t::operator<=(const pick_param_str_t &other) const {
  return strcmp(str_, other.str_) <= 0;
}
bool test_manager::pick_param_str_t::operator<(const pick_param_str_t &other) const {
  return strcmp(str_, other.str_) < 0;
}
#endif

test_manager::test_manager() {
  success_ = 0;
  failed_ = 0;
}

test_manager::~test_manager() {}

void test_manager::append_test_case(const std::string &test_name, const std::string &case_name, case_ptr_type ptr) {
  tests_[test_name].push_back(std::make_pair(case_name, ptr));
}

void test_manager::append_event_on_start(const std::string &event_name, on_start_ptr_type ptr) {
  evt_on_starts_.push_back(std::make_pair(event_name, ptr));
}

void test_manager::append_event_on_exit(const std::string &event_name, on_exit_ptr_type ptr) {
  evt_on_exits_.push_back(std::make_pair(event_name, ptr));
}

#ifdef UTILS_TEST_MACRO_TEST_ENABLE_BOOST_TEST

boost::unit_test::test_suite *&test_manager::test_suit() {
  static boost::unit_test::test_suite *ret = nullptr;
  return ret;
}

int test_manager::run() {
  using namespace boost::unit_test;

  for (test_data_type::iterator iter = tests_.begin(); iter != tests_.end(); ++iter) {
    test_suit() = BOOST_TEST_SUITE(iter->first.c_str());

    for (test_type::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2) {
      test_suit()->add(make_test_case(callback0<>(iter2->second->func_), iter2->first.c_str()));
      iter2->second->run();
    }

    framework::master_test_suite().add(test_suit());
  }

  return 0;
}

#else

static void topological_sort(UTIL_UNIT_TEST_MACRO_AUTO_MAP(std::string, detail::topological_sort_object_t) & in,
                             std::vector<detail::topological_sort_object_t *> &out) {
  typedef UTIL_UNIT_TEST_MACRO_AUTO_MAP(std::string, detail::topological_sort_object_t) index_by_name_t;
  out.reserve(in.size());

  for (index_by_name_t::iterator iter = in.begin(); iter != in.end(); ++iter) {
    if (0 == iter->second.dependency_count) {
      out.push_back(&iter->second);
    }
  }

  for (size_t i = 0; i < out.size(); ++i) {
    for (std::list<detail::topological_sort_object_t *>::iterator iter = out[i]->depend_by.begin();
         iter != out[i]->depend_by.end(); ++iter) {
      if ((*iter)->dependency_count > 0) {
        --(*iter)->dependency_count;

        if (0 == (*iter)->dependency_count) {
          out.push_back(*iter);
        }
      }
    }
  }
}

int test_manager::run_event_on_start() {
  // generate topological_sort_object_t
  typedef UTIL_UNIT_TEST_MACRO_AUTO_MAP(std::string, detail::topological_sort_object_t) index_by_name_t;
  index_by_name_t index_by_name;
  for (size_t i = 0; i < evt_on_starts_.size(); ++i) {
    detail::topological_sort_object_t &obj = index_by_name[evt_on_starts_[i].first];
    obj.name = evt_on_starts_[i].first;
    obj.object = reinterpret_cast<void *>(evt_on_starts_[i].second);
    obj.dependency_count = 0;
  }

  for (size_t i = 0; i < evt_on_starts_.size(); ++i) {
    detail::topological_sort_object_t &obj = index_by_name[evt_on_starts_[i].first];

    for (test_on_start_base::after_set_t::iterator iter = evt_on_starts_[i].second->after.begin();
         iter != evt_on_starts_[i].second->after.end(); ++iter) {
      index_by_name_t::iterator dep_iter = index_by_name.find(*iter);
      if (dep_iter == index_by_name.end()) {
        util::cli::shell_stream ss(std::cerr);
        ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_GREEN << util::cli::shell_font_style::SHELL_FONT_SPEC_BOLD
             << "[ WARNING  ] " << util::cli::shell_font_style::SHELL_FONT_SPEC_NULL << "On Start Event "
             << evt_on_starts_[i].first << " is configured run after " << (*iter) << ", but " << (*iter) << "not found."
             << std::endl;
        continue;
      }

      ++obj.dependency_count;
      dep_iter->second.depend_by.push_back(&obj);
    }
  }

  std::vector<detail::topological_sort_object_t *> run_order;
  topological_sort(index_by_name, run_order);

  for (size_t i = 0; i < run_order.size(); ++i) {
    util::cli::shell_stream ss(std::cout);
    ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_GREEN << util::cli::shell_font_style::SHELL_FONT_SPEC_BOLD
         << "[ On Start ] " << util::cli::shell_font_style::SHELL_FONT_SPEC_NULL << "Running " << run_order[i]->name
         << std::endl;
    reinterpret_cast<on_start_ptr_type>(run_order[i]->object)->run();
  }

  return 0;
}

int test_manager::run_event_on_exit() {
  typedef UTIL_UNIT_TEST_MACRO_AUTO_MAP(std::string, detail::topological_sort_object_t) index_by_name_t;
  // generate topological_sort_object_t
  index_by_name_t index_by_name;
  for (size_t i = 0; i < evt_on_exits_.size(); ++i) {
    detail::topological_sort_object_t &obj = index_by_name[evt_on_exits_[i].first];
    obj.name = evt_on_exits_[i].first;
    obj.object = reinterpret_cast<void *>(evt_on_exits_[i].second);
    obj.dependency_count = 0;
  }

  for (size_t i = 0; i < evt_on_exits_.size(); ++i) {
    detail::topological_sort_object_t &obj = index_by_name[evt_on_exits_[i].first];

    for (test_on_exit_base::before_set_t::iterator iter = evt_on_exits_[i].second->before.begin();
         iter != evt_on_exits_[i].second->before.end(); ++iter) {
      index_by_name_t::iterator dep_iter = index_by_name.find(*iter);
      if (dep_iter == index_by_name.end()) {
        util::cli::shell_stream ss(std::cerr);
        ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_GREEN << util::cli::shell_font_style::SHELL_FONT_SPEC_BOLD
             << "[ WARNING  ] " << util::cli::shell_font_style::SHELL_FONT_SPEC_NULL << "On Exit Event "
             << evt_on_exits_[i].first << " is configured run before " << (*iter) << ", but " << (*iter) << "not found."
             << std::endl;
        continue;
      }

      ++obj.dependency_count;
      dep_iter->second.depend_by.push_back(&obj);
    }
  }

  std::vector<detail::topological_sort_object_t *> run_order;
  topological_sort(index_by_name, run_order);

  for (size_t i = 0; i < run_order.size(); ++i) {
    size_t idx = run_order.size() - 1 - i;
    util::cli::shell_stream ss(std::cout);
    ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_GREEN << util::cli::shell_font_style::SHELL_FONT_SPEC_BOLD
         << "[ On Exit  ] " << util::cli::shell_font_style::SHELL_FONT_SPEC_NULL << "Running " << run_order[idx]->name
         << std::endl;
    reinterpret_cast<on_exit_ptr_type>(run_order[idx]->object)->run();
  }

  return 0;
}

int test_manager::run() {
  success_ = 0;
  failed_ = 0;

  clock_t all_begin_time = clock();
  util::cli::shell_stream ss(std::cout);
  ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_GREEN << util::cli::shell_font_style::SHELL_FONT_SPEC_BOLD
       << "[==========] " << util::cli::shell_font_style::SHELL_FONT_SPEC_NULL << "Running " << tests_.size()
       << " test(s)" << std::endl;

  for (test_data_type::iterator iter = tests_.begin(); iter != tests_.end(); ++iter) {
    bool check_test_group_passed = run_cases_.empty();
    bool check_test_group_has_cases = false;

    if (!check_test_group_passed) {
      check_test_group_passed = run_cases_.end() != run_cases_.find(iter->first);
    }

    if (!check_test_group_passed) {
      check_test_group_has_cases = run_groups_.end() != run_groups_.find(iter->first);
    }

    // skip unknown groups
    if (!check_test_group_passed && !check_test_group_has_cases) {
      continue;
    }

    size_t run_group_count = 0;

    ss() << std::endl
         << util::cli::shell_font_style::SHELL_FONT_COLOR_GREEN << util::cli::shell_font_style::SHELL_FONT_SPEC_BOLD
         << "[----------] " << util::cli::shell_font_style::SHELL_FONT_SPEC_NULL << iter->second.size()
         << " test case(s) from " << iter->first << std::endl;

    clock_t test_begin_time = clock();
    for (test_type::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2) {
      bool check_test_case_passed = run_cases_.empty() || check_test_group_passed;
      if (!check_test_case_passed) {
        check_test_case_passed = run_cases_.end() != run_cases_.find(iter2->first);

        if (!check_test_case_passed) {
          std::string full_name;
          full_name.reserve(iter->first.size() + 1 + iter2->first.size());
          full_name = iter->first + "." + iter2->first;
          check_test_case_passed = run_cases_.end() != run_cases_.find(full_name);
        }
      }

      // skip unknown cases
      if (!check_test_case_passed) {
        continue;
      }

      ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_GREEN << "[ RUN      ] "
           << util::cli::shell_font_style::SHELL_FONT_SPEC_NULL << iter->first << "." << iter2->first << std::endl;

      clock_t case_begin_time = clock();
      iter2->second->run();
      clock_t case_end_time = clock();

      if (0 == iter2->second->failed_) {
        ++success_;
        ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_GREEN << "[       OK ] "
             << util::cli::shell_font_style::SHELL_FONT_SPEC_NULL << iter->first << "." << iter2->first << " ("
             << get_expire_time(case_begin_time, case_end_time) << ")" << std::endl;
      } else {
        ++failed_;
        ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_RED << "[  FAILED  ] "
             << util::cli::shell_font_style::SHELL_FONT_SPEC_NULL << iter->first << "." << iter2->first << " ("
             << get_expire_time(case_begin_time, case_end_time) << ")" << std::endl;
      }

      ++run_group_count;
    }

    clock_t test_end_time = clock();
    ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_GREEN << util::cli::shell_font_style::SHELL_FONT_SPEC_BOLD
         << "[----------] " << util::cli::shell_font_style::SHELL_FONT_SPEC_NULL << run_group_count
         << " test case(s) from " << iter->first << " (" << get_expire_time(test_begin_time, test_end_time) << " total)"
         << std::endl;
  }

  clock_t all_end_time = clock();
  ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_GREEN << util::cli::shell_font_style::SHELL_FONT_SPEC_BOLD
       << "[==========] " << util::cli::shell_font_style::SHELL_FONT_SPEC_NULL << (success_ + failed_)
       << " test(s) ran."
       << " (" << get_expire_time(all_begin_time, all_end_time) << " total)" << std::endl;

  ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_GREEN << "[  PASSED  ] "
       << util::cli::shell_font_style::SHELL_FONT_SPEC_NULL << success_ << " test case(s)." << std::endl;

  if (failed_ > 0) {
    ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_RED << "[  FAILED  ] "
         << util::cli::shell_font_style::SHELL_FONT_SPEC_NULL << failed_ << " test case(s), listed below:" << std::endl;

    for (test_data_type::iterator iter = tests_.begin(); iter != tests_.end(); ++iter) {
      for (test_type::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2) {
        if (iter2->second->failed_ > 0) {
          ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_RED << "[  FAILED  ] "
               << util::cli::shell_font_style::SHELL_FONT_SPEC_NULL << iter->first << "." << iter2->first << std::endl;
        }
      }
    }
  }

  return (0 == failed_) ? 0 : -failed_;
}

#endif

void test_manager::set_cases(const std::vector<std::string> &case_names) {
  run_cases_.clear();
  run_groups_.clear();

  for (size_t i = 0; i < case_names.size(); ++i) {
    run_cases_.insert(case_names[i]);
    std::string::size_type split_idx = case_names[i].find('.');
    if (split_idx != std::string::npos) {
      run_groups_.insert(case_names[i].substr(0, split_idx));
    }
  }
}

test_manager &test_manager::me() {
  static test_manager ret;
  return ret;
}

std::string test_manager::get_expire_time(clock_t begin, clock_t end) {
  std::stringstream ss;
  double ms = 1000.0 * (end - begin) / CLOCKS_PER_SEC;

  ss << ms << " ms";

  return ss.str();
}

void test_manager::set_counter_ptr(int *success_counter_ptr, int *failed_counter_ptr) {
  detail::test_manager_tls_block_t *block = detail::get_test_manager_tls_block();
  if (nullptr != block) {
    block->success_counter_ptr = success_counter_ptr;
    block->failed_counter_ptr = failed_counter_ptr;
  }
  detail::g_global_counter_cache.success_counter_ptr = success_counter_ptr;
  detail::g_global_counter_cache.failed_counter_ptr = failed_counter_ptr;
}

void test_manager::inc_success_counter() {
  detail::test_manager_tls_block_t *block = detail::get_test_manager_tls_block();
  COPP_LIKELY_IF (nullptr != block && nullptr != block->success_counter_ptr) {
    ++(*block->success_counter_ptr);
    return;
  }

  util::cli::shell_stream ss(std::cerr);
  ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_RED << util::cli::shell_font_style::SHELL_FONT_SPEC_BOLD
       << "[==========] "
       << "Expect expression can not be used when not running test case." << std::endl;
}

void test_manager::inc_failed_counter() {
  detail::test_manager_tls_block_t *block = detail::get_test_manager_tls_block();
  COPP_LIKELY_IF (nullptr != block && nullptr != block->failed_counter_ptr) {
    ++(*block->failed_counter_ptr);
    return;
  }

  util::cli::shell_stream ss(std::cerr);
  ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_RED << util::cli::shell_font_style::SHELL_FONT_SPEC_BOLD
       << "[==========] "
       << "Expect expression can not be used when not running test case." << std::endl;
}

int run_event_on_start() { return test_manager::me().run_event_on_start(); }

int run_event_on_exit() { return test_manager::me().run_event_on_exit(); }

int run_tests(int argc, char *argv[]) {
  std::vector<std::string> run_cases;
  const char *version = "1.0.0";
  bool is_help = false;
  bool is_show_version = false;

  util::cli::cmd_option::ptr_type cmd_opts = util::cli::cmd_option::create();
  cmd_opts->bind_cmd("-h, --help, help", util::cli::phoenix::set_const(is_help, true))
      ->set_help_msg("                              show help message and exit.");
  cmd_opts->bind_cmd("-v, --version, version", util::cli::phoenix::set_const(is_show_version, true))
      ->set_help_msg("                              show version and exit.");
  cmd_opts->bind_cmd("-r, --run, run", util::cli::phoenix::push_back(run_cases))
      ->set_help_msg("[case names...]               only run specify cases.");

  cmd_opts->start(argc, argv);
  if (is_help) {
    std::cout << *cmd_opts << std::endl;
    return 0;
  }

  if (is_show_version) {
    std::cout << version << std::endl;
    return 0;
  }

  test_manager::me().set_cases(run_cases);
  run_event_on_start();
  int ret = test_manager::me().run();
  run_event_on_exit();
  return ret;
}
