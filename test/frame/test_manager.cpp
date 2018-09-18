/*
 * test_manager.cpp
 *
 *  Created on: 2014年3月11日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "cli/cmd_option.h"
#include "cli/cmd_option_phoenix.h"
#include "cli/shell_font.h"

#include "test_manager.h"


test_manager::pick_param_str_t::pick_param_str_t(const char *in) : str_(in) {}
test_manager::pick_param_str_t::pick_param_str_t(const std::string &in) : str_(in.c_str()) {}

bool test_manager::pick_param_str_t::operator==(const pick_param_str_t &other) const { return strcmp(str_, other.str_) == 0; }
bool test_manager::pick_param_str_t::operator!=(const pick_param_str_t &other) const { return strcmp(str_, other.str_) != 0; }
bool test_manager::pick_param_str_t::operator>=(const pick_param_str_t &other) const { return strcmp(str_, other.str_) >= 0; }
bool test_manager::pick_param_str_t::operator>(const pick_param_str_t &other) const { return strcmp(str_, other.str_) > 0; }
bool test_manager::pick_param_str_t::operator<=(const pick_param_str_t &other) const { return strcmp(str_, other.str_) <= 0; }
bool test_manager::pick_param_str_t::operator<(const pick_param_str_t &other) const { return strcmp(str_, other.str_) < 0; }

test_manager::test_manager() {
    success_counter_ptr = failed_counter_ptr = NULL;
    success_                                 = 0;
    failed_                                  = 0;
}

test_manager::~test_manager() {}

void test_manager::append(const std::string &test_name, const std::string &case_name, case_ptr_type ptr) {
    tests_[test_name].push_back(std::make_pair(case_name, ptr));
}

#ifdef UTILS_TEST_MACRO_TEST_ENABLE_BOOST_TEST

boost::unit_test::test_suite *&test_manager::test_suit() {
    static boost::unit_test::test_suite *ret = NULL;
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

int test_manager::run() {
    success_ = 0;
    failed_  = 0;

    clock_t                 all_begin_time = clock();
    util::cli::shell_stream ss(std::cout);
    ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_GREEN << util::cli::shell_font_style::SHELL_FONT_SPEC_BOLD << "[==========] "
         << util::cli::shell_font_style::SHELL_FONT_SPEC_NULL << "Running " << tests_.size() << " test(s)" << std::endl;

    for (test_data_type::iterator iter = tests_.begin(); iter != tests_.end(); ++iter) {
        bool check_test_group_passed    = run_cases_.empty();
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
             << util::cli::shell_font_style::SHELL_FONT_COLOR_GREEN << util::cli::shell_font_style::SHELL_FONT_SPEC_BOLD << "[----------] "
             << util::cli::shell_font_style::SHELL_FONT_SPEC_NULL << iter->second.size() << " test case(s) from " << iter->first
             << std::endl;

        clock_t test_begin_time = clock();
        for (test_type::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2) {
            bool check_test_case_passed = run_cases_.empty() || check_test_group_passed;
            if (!check_test_case_passed) {
                check_test_case_passed = run_cases_.end() != run_cases_.find(iter2->first);

                if (!check_test_case_passed) {
                    std::string full_name;
                    full_name.reserve(iter->first.size() + 1 + iter2->first.size());
                    full_name              = iter->first + "." + iter2->first;
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
        ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_GREEN << util::cli::shell_font_style::SHELL_FONT_SPEC_BOLD << "[----------] "
             << util::cli::shell_font_style::SHELL_FONT_SPEC_NULL << run_group_count << " test case(s) from " << iter->first << " ("
             << get_expire_time(test_begin_time, test_end_time) << " total)" << std::endl;
    }

    clock_t all_end_time = clock();
    ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_GREEN << util::cli::shell_font_style::SHELL_FONT_SPEC_BOLD << "[==========] "
         << util::cli::shell_font_style::SHELL_FONT_SPEC_NULL << (success_ + failed_) << " test(s) ran."
         << " (" << get_expire_time(all_begin_time, all_end_time) << " total)" << std::endl;

    ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_GREEN << "[  PASSED  ] " << util::cli::shell_font_style::SHELL_FONT_SPEC_NULL
         << success_ << " test case(s)." << std::endl;

    if (failed_ > 0) {
        ss() << util::cli::shell_font_style::SHELL_FONT_COLOR_RED << "[  FAILED  ] " << util::cli::shell_font_style::SHELL_FONT_SPEC_NULL
             << failed_ << " test case(s), listed below:" << std::endl;

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
    double            ms = 1000.0 * (end - begin) / CLOCKS_PER_SEC;

    ss << ms << " ms";

    return ss.str();
}

int run_tests(int argc, char *argv[]) {
    std::vector<std::string> run_cases;
    const char *             version         = "1.0.0";
    bool                     is_help         = false;
    bool                     is_show_version = false;

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
    return test_manager::me().run();
}
