/*
 * test_manager.cpp
 *
 *  Created on: 2014年3月11日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#include <iostream>
#include <sstream>

#include "shell_font.h"

#include "test_manager.h"

test_manager::test_manager() {
    success_counter_ptr = failed_counter_ptr = NULL;
    success_ = 0;
    failed_ = 0;
}

test_manager::~test_manager() {
}

void test_manager::append(const std::string& test_name, const std::string& case_name, case_ptr_type ptr) {
    tests_[test_name].push_back(std::make_pair(case_name, ptr));
}

#ifdef UTILS_TEST_MACRO_TEST_ENABLE_BOOST_TEST

boost::unit_test::test_suite*& test_manager::test_suit() {
    static boost::unit_test::test_suite* ret = NULL;
    return ret;
}

int test_manager::run() {
    using namespace boost::unit_test;

    for(test_data_type::iterator iter = tests_.begin(); iter != tests_.end(); ++ iter) {
        test_suit() = BOOST_TEST_SUITE(iter->first.c_str());

        for (test_type::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); ++ iter2){
            test_suit()->add(make_test_case(
                callback0<>(iter2->second->func_),
                iter2->first.c_str()
            ));
            iter2->second->run();
        }

        framework::master_test_suite().add(test_suit());
    }

    return 0;
}

#else

int test_manager::run() {
    success_ = 0;
    failed_ = 0;

    clock_t all_begin_time = clock();
    shell_stream ss(std::cout);
    ss() << ShekkFontStyle::SHELL_FONT_COLOR_GREEN << ShekkFontStyle::SHELL_FONT_SPEC_BOLD <<
        "[==========] " << ShekkFontStyle::SHELL_FONT_SPEC_NULL << 
        "Running " << tests_.size() << " test(s)" <<
        std::endl;

    for(test_data_type::iterator iter = tests_.begin(); iter != tests_.end(); ++ iter) {
        ss() << std::endl << ShekkFontStyle::SHELL_FONT_COLOR_GREEN << ShekkFontStyle::SHELL_FONT_SPEC_BOLD <<
            "[----------] " << ShekkFontStyle::SHELL_FONT_SPEC_NULL << iter->second.size() << " test case(s) from " << iter->first <<
            std::endl;

        clock_t test_begin_time = clock();
        for (test_type::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); ++ iter2) {
            ss() << ShekkFontStyle::SHELL_FONT_COLOR_GREEN << 
                "[ RUN      ] " << ShekkFontStyle::SHELL_FONT_SPEC_NULL <<
                iter->first << "." << iter2->first << std::endl;

            clock_t case_begin_time = clock();
            iter2->second->run();
            clock_t case_end_time = clock();

            if (0 == iter2->second->failed_) {
                ++ success_;
                ss() << ShekkFontStyle::SHELL_FONT_COLOR_GREEN<< 
                    "[       OK ] " << ShekkFontStyle::SHELL_FONT_SPEC_NULL <<
                    iter->first << "." << iter2->first << " (" << get_expire_time(case_begin_time, case_end_time) << ")" <<
                    std::endl;
            } else {
                ++ failed_;
                ss() << ShekkFontStyle::SHELL_FONT_COLOR_RED<<
                    "[  FAILED  ] " << ShekkFontStyle::SHELL_FONT_SPEC_NULL<<
                    iter->first << "." << iter2->first << " (" << get_expire_time(case_begin_time, case_end_time) << ")" <<
                    std::endl;
            }
        }

        clock_t test_end_time = clock();
        ss() << ShekkFontStyle::SHELL_FONT_COLOR_GREEN << ShekkFontStyle::SHELL_FONT_SPEC_BOLD<<
            "[----------] " << ShekkFontStyle::SHELL_FONT_SPEC_NULL <<
            iter->second.size() << " test case(s) from " << iter->first << " (" << get_expire_time(test_begin_time, test_end_time) << " total)" <<
            std::endl;

    }

    clock_t all_end_time = clock();
    ss() << ShekkFontStyle::SHELL_FONT_COLOR_GREEN << ShekkFontStyle::SHELL_FONT_SPEC_BOLD <<
        "[==========] " << ShekkFontStyle::SHELL_FONT_SPEC_NULL <<
        tests_.size() << " test(s) ran." << " (" << get_expire_time(all_begin_time, all_end_time) << " total)" <<
        std::endl;

    ss() << ShekkFontStyle::SHELL_FONT_COLOR_GREEN << 
        "[  PASSED  ] " << ShekkFontStyle::SHELL_FONT_SPEC_NULL <<
        success_ << " test case(s)." <<
        std::endl;

    if (failed_ > 0) {
        ss() << ShekkFontStyle::SHELL_FONT_COLOR_RED <<
            "[  FAILED  ] " << ShekkFontStyle::SHELL_FONT_SPEC_NULL <<
            failed_ << " test case(s), listed below:" <<
            std::endl;

        for(test_data_type::iterator iter = tests_.begin(); iter != tests_.end(); ++ iter) {
            for (test_type::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); ++ iter2){
                if (iter2->second->failed_ > 0) {
                    ss() << ShekkFontStyle::SHELL_FONT_COLOR_RED <<
                        "[  FAILED  ] " << ShekkFontStyle::SHELL_FONT_SPEC_NULL <<
                        iter->first << "." << iter2->first <<
                        std::endl;
                }
            }
        }
    }

    return (0 == failed_)? 0: -failed_;
}

#endif

test_manager& test_manager::me() {
    static test_manager ret;
    return ret;
}

std::string test_manager::get_expire_time(clock_t begin, clock_t end) {
    std::stringstream ss;
    double ms = 1000.0 * (end - begin) / CLOCKS_PER_SEC;

    ss << ms<< " ms";

    return ss.str();
}

int run_tests() {
    return test_manager::me().run();
}

