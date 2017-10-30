/*
 * test_case_base.h
 *
 *  Created on: 2014年3月11日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef TEST_CASE_BASE_H_
#define TEST_CASE_BASE_H_

#pragma once

#include <string>

#ifdef UTILS_TEST_MACRO_TEST_ENABLE_BOOST_TEST
#include <boost/test/unit_test.hpp>
#endif

/*
 *
 */
class test_case_base {
public:
    typedef void (*test_func)();

public:
    test_case_base(const std::string& test_name, const std::string& case_name, test_func func);
    virtual ~test_case_base();

    virtual int run();

    int success_;
    int failed_;

    test_func func_;
};

#endif /* TEST_CASE_BASE_H_ */
