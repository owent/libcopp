/*
 * test_case_base.cpp
 *
 *  Created on: 2014年3月11日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#include "test_manager.h"

#include "test_case_base.h"

test_case_base::test_case_base(const std::string& test_name, const std::string& case_name, test_func func) {
  success_ = 0;
  failed_ = 0;

  func_ = func;

  test_manager::me().append_test_case(test_name, case_name, this);
}

test_case_base::~test_case_base() {}

int test_case_base::run() {
  success_ = 0;
  failed_ = 0;

  test_manager::set_counter_ptr(&success_, &failed_);

#ifndef UTILS_TEST_MACRO_TEST_ENABLE_BOOST_TEST
  if (NULL != func_) {
    (*func_)();
  }
#endif

  test_manager::set_counter_ptr(NULL, NULL);

  return 0;
}

void test_on_start_base::register_self() { test_manager::me().append_event_on_start(name, this); }

test_on_start_base::~test_on_start_base() {}

int test_on_start_base::run() {
  if (NULL != func_) {
    (*func_)();
  }
  return 0;
}

void test_on_exit_base::register_self() { test_manager::me().append_event_on_exit(name, this); }

test_on_exit_base::~test_on_exit_base() {}

int test_on_exit_base::run() {
  if (NULL != func_) {
    (*func_)();
  }
  return 0;
}