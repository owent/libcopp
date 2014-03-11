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
    success_ = 0;
    failed_ = 0;
}

test_manager::~test_manager() {
}

void test_manager::append(const std::string& test_name, const std::string& case_name, case_ptr_type ptr) {
    tests_[test_name].push_back(std::make_pair(case_name, ptr));
}

int test_manager::run() {
    success_ = 0;
    failed_ = 0;

    clock_t all_begin_time = clock();
    std::cout<< shell_font::GenerateString("[==========] ", SHELL_FONT_COLOR_GREEN | SHELL_FONT_SPEC_BOLD)<<
        "Running "<< tests_.size()<< " test(s)"<<
        std::endl;

    for(test_data_type::iterator iter = tests_.begin(); iter != tests_.end(); ++ iter) {
        std::cout<< std::endl<< shell_font::GenerateString("[----------] ", SHELL_FONT_COLOR_GREEN | SHELL_FONT_SPEC_BOLD)<<
            iter->second.size()<< " test case(s) from "<< iter->first<<
            std::endl;

        clock_t test_begin_time = clock();
        for (test_type::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); ++ iter2){
            std::cout<< shell_font::GenerateString("[ RUN      ] ", SHELL_FONT_COLOR_GREEN)<<
                iter->first<< "."<< iter2->first<<
                std::endl;

            clock_t case_begin_time = clock();
            iter2->second->run();
            clock_t case_end_time = clock();

            if (0 == iter2->second->failed_) {
                ++ success_;
                std::cout<< shell_font::GenerateString("[       OK ] ", SHELL_FONT_COLOR_GREEN)<<
                    iter->first<< "."<< iter2->first<< " (" << get_expire_time(case_begin_time, case_end_time)<< ")"<<
                    std::endl;
            } else {
                ++ failed_;
                std::cout<< shell_font::GenerateString("[  FAILED  ] ", SHELL_FONT_COLOR_RED)<<
                    iter->first<< "."<< iter2->first<< " (" << get_expire_time(case_begin_time, case_end_time)<< ")"<<
                    std::endl;
            }
        }

        clock_t test_end_time = clock();
        std::cout<< shell_font::GenerateString("[----------] ", SHELL_FONT_COLOR_GREEN | SHELL_FONT_SPEC_BOLD)<<
            iter->second.size()<< " test case(s) from "<< iter->first<< " (" << get_expire_time(test_begin_time, test_end_time)<< " total)"<<
            std::endl;

    }

    clock_t all_end_time = clock();
    std::cout<< shell_font::GenerateString("[==========] ", SHELL_FONT_COLOR_GREEN | SHELL_FONT_SPEC_BOLD)<<
        tests_.size()<< " test(s) ran."<< " (" << get_expire_time(all_begin_time, all_end_time)<< " total)"<<
        std::endl;

    std::cout<< shell_font::GenerateString("[  PASSED  ] ", SHELL_FONT_COLOR_GREEN)<<
        success_<< " test(s)."<<
        std::endl;

    if (failed_ > 0) {
        std::cout<< shell_font::GenerateString("[  FAILED  ] ", SHELL_FONT_COLOR_RED)<<
            failed_<< " test(s), listed below:"<<
            std::endl;

        for(test_data_type::iterator iter = tests_.begin(); iter != tests_.end(); ++ iter) {
            for (test_type::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); ++ iter2){
                if (iter2->second->failed_ > 0) {
                    std::cout<< shell_font::GenerateString("[  FAILED  ] ", SHELL_FONT_COLOR_RED)<<
                        iter->first<< "."<< iter2->first<<
                        std::endl;
                }
            }
        }
    }

    return (0 == failed_)? 0: -failed_;
}

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

