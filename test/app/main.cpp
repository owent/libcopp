/*
 * main.cpp
 *
 *  Created on: 2014年3月11日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#include "frame/test_macros.h"

#if defined(UTILS_TEST_MACRO_TEST_ENABLE_BOOST_TEST)

#ifdef BOOST_TEST_ALTERNATIVE_INIT_API
bool init_unit_test() {
#else
boost::unit_test::test_suite *init_unit_test_suite(int argc, char *argv[]) {
#endif

    run_tests(argc, argv);

#ifdef BOOST_TEST_ALTERNATIVE_INIT_API
    return true;
#else
    return 0;
#endif
}

#endif


#if !defined(UTILS_TEST_MACRO_TEST_ENABLE_BOOST_TEST) || defined(BOOST_TEST_DYN_LINK) || defined(BOOST_TEST_NO_MAIN)

int main(int argc, char *argv[]) {

#ifdef UTILS_TEST_MACRO_TEST_ENABLE_GTEST
    ::testing::InitGoogleTest(&argc, argv);
    run_event_on_start();
    int ret = RUN_ALL_TESTS();
    run_event_on_exit();
    return ret;
#elif defined(UTILS_TEST_MACRO_TEST_ENABLE_BOOST_TEST)
// prototype for user's unit test init function
#ifdef BOOST_TEST_ALTERNATIVE_INIT_API
    boost::unit_test::init_unit_test_func init_func = &init_unit_test;
#else
    boost::unit_test::init_unit_test_func init_func = &init_unit_test_suite;
#endif

    run_event_on_start();
    int ret = ::boost::unit_test::unit_test_main(init_func, argc, argv);
    run_event_on_exit();
    return ret;
#else

    return run_tests(argc, argv);
#endif
}

#endif
