# =========== libcopp/src - stack_context =========== 
set (PROJECT_LIBCOPP_STACK_CONTEXT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}")
set (PROJECT_LIBCOPP_STACK_ALLOC_SRC_DIR "${PROJECT_LIBCOPP_STACK_CONTEXT_SRC_DIR}/allocator")

# ========== stack information ==========
aux_source_directory("${PROJECT_LIBCOPP_STACK_CONTEXT_SRC_DIR}" SRC_LIST)
list(APPEND COPP_SRC_LIST ${SRC_LIST})

# ========== stack allocator ==========
list(APPEND COPP_SRC_LIST "${PROJECT_LIBCOPP_STACK_ALLOC_SRC_DIR}/stack_allocator_memory.cpp")
list(APPEND COPP_SRC_LIST "${PROJECT_LIBCOPP_STACK_ALLOC_SRC_DIR}/stack_allocator_malloc.cpp")

if(WIN32 OR WINCE OR WINDOWS_PHONE OR WINDOWS_STORE)
	EchoWithColor(COLOR GREEN "-- stack allocator: enable windows allocator")
	list(APPEND COPP_SRC_LIST "${PROJECT_LIBCOPP_STACK_ALLOC_SRC_DIR}/stack_allocator_windows.cpp")
	add_compiler_define(COPP_MACRO_SYS_WIN=1)
endif()

if(NOT WIN32 AND UNIX)
	EchoWithColor(COLOR GREEN "-- stack allocator: enable posix allocator")
	list(APPEND COPP_SRC_LIST "${PROJECT_LIBCOPP_STACK_ALLOC_SRC_DIR}/stack_allocator_posix.cpp")
	add_compiler_define(COPP_MACRO_SYS_POSIX=1)
endif()

if(COPP_MACRO_USE_SEGMENTED_STACKS_GCC)
	EchoWithColor(COLOR GREEN "-- gcc ${CMAKE_CXX_COMPILER_VERSION} found in unix like system.")
	EchoWithColor(COLOR GREEN "-- stack allocator: enable segment allocator, add -DCOPP_MACRO_USE_SEGMENTED_STACKS")
	add_definitions(-DCOPP_MACRO_USE_SEGMENTED_STACKS)
	list(APPEND COPP_SRC_LIST "${PROJECT_LIBCOPP_STACK_ALLOC_SRC_DIR}/stack_allocator_split_segment.cpp")
endif()

if (COPP_MACRO_USE_VALGRIND)
    EchoWithColor(COLOR GREEN "-- gcc enable valgrind.")
	add_definitions(-DCOPP_MACRO_USE_VALGRIND)
endif()
