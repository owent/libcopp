/*
 * test_manager.h
 *
 *  Created on: 2014年3月11日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef TEST_MANAGER_H_
#define TEST_MANAGER_H_

#include <string>
#include <map>
#include <vector>
#include <ctime>

#include "test_case_base.h"

/*
 *
 */
class test_manager {
public:
    typedef test_case_base* case_ptr_type;
    typedef std::vector<std::pair<std::string, case_ptr_type> > test_type;
    typedef std::map<std::string, test_type> test_data_type;

public:
    test_manager();
    virtual ~test_manager();

    void append(const std::string& test_name, const std::string& case_name, case_ptr_type);

    int run();

    static test_manager& me();

    int* success_counter_ptr;
    int* failed_counter_ptr;

    static std::string get_expire_time(clock_t begin, clock_t end);

#ifdef UTILS_TEST_MACRO_TEST_ENABLE_BOOST_TEST
    static boost::unit_test::test_suite*& test_suit();
#endif

private:
    test_data_type tests_;
    int success_;
    int failed_;
};

int run_tests();

#endif /* TEST_MANAGER_H_ */
