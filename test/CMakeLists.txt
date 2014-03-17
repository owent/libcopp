
EchoWithColor(COLOR GREEN "-- Configure Test Projects")

enable_testing()
find_package(GTest)
set (PROJECT_LIBCOPP_TEST_LIB_LINK "")

# =========== find gtest ===========
if (GTEST_FOUND)
    EchoWithColor(COLOR GREEN "-- GTest Found: ${GTEST_LIBRARIES}")
	if ( NOT MSVC )
        add_definitions(-DCOPP_MACRO_TEST_ENABLE_GTEST=1)
		set (PROJECT_LIBCOPP_TEST_LIB_LINK ${GTEST_LIBRARIES} pthread)
	else()
	    add_definitions(/D COPP_MACRO_TEST_ENABLE_GTEST=1)
		set (PROJECT_LIBCOPP_TEST_LIB_LINK ${GTEST_LIBRARIES})
	endif()
    include_directories(${GTEST_INCLUDE_DIRS})
endif()

include_directories(${PROJECT_TEST_INC_DIR})

file(GLOB_RECURSE COPP_TEST_SRC_LIST 
    ${PROJECT_TEST_SRC_DIR}/*.h 
    ${PROJECT_TEST_SRC_DIR}/*.hpp 
    ${PROJECT_TEST_SRC_DIR}/*.c 
    ${PROJECT_TEST_SRC_DIR}/*.cpp
    ${PROJECT_TEST_SRC_DIR}/*.cc 
    ${PROJECT_TEST_SRC_DIR}/*.cxx
)
source_group_by_dir(COPP_TEST_SRC_LIST)


# ============ test - coroutine test frame ============
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/test")

add_executable(coroutine_test ${COPP_TEST_SRC_LIST})
target_link_libraries(coroutine_test ${PROJECT_LIBCOPP_LIB_LINK} ${PROJECT_LIBCOPP_TEST_LIB_LINK})

add_test(test coroutine_test)
