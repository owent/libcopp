

EchoWithColor(COLOR GREEN "-- Configure Test Projects")

# ============ test - coroutine_context_base_test ============
add_executable(coroutine_context_base_test "${PROJECT_TEST_SRC_DIR}/coroutine_context_base_test.cpp")
target_link_libraries(coroutine_context_base_test ${PROJECT_LIBCOPP_LIB_LINK})

# ============ test - fcontext_test ============
add_executable(fcontext_test "${PROJECT_TEST_SRC_DIR}/fcontext_test.cpp")
target_link_libraries(fcontext_test ${PROJECT_LIBCOPP_LIB_LINK})

# ============ test - coroutine_context_split_segment_test ============
if(COPP_MACRO_USE_SEGMENTED_STACKS)
	add_executable(coroutine_context_split_segment "${PROJECT_TEST_SRC_DIR}/coroutine_context_split_segment.cpp")
	target_link_libraries(coroutine_context_split_segment ${PROJECT_LIBCOPP_LIB_LINK})
	EchoWithColor(COLOR GREEN "-- segmented stack enabled, add test - coroutine_context_split_segment_test.")
endif()
