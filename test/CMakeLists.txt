

EchoWithColor(COLOR GREEN "-- Configure Test Projects")

# ============ test - coroutine_context_base_test ============
add_executable(coroutine_context_base_test "${PROJECT_TEST_SRC_DIR}/coroutine_context_base_test.cpp")
target_link_libraries(coroutine_context_base_test ${PROJECT_LIBCOPP_LIB_LINK})

# ============ test - fcontext_test ============
add_executable(fcontext_test "${PROJECT_TEST_SRC_DIR}/fcontext_test.cpp")
target_link_libraries(fcontext_test ${PROJECT_LIBCOPP_LIB_LINK})
