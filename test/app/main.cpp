/*
 * main.cpp
 *
 *  Created on: 2014年3月11日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#include "frame/test_macros.h"

int main(int argc, char *argv[]) {

#ifdef COPP_MACRO_TEST_ENABLE_GTEST
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
#else
    return run_tests();
#endif

}


