# =========== libcopp/src - stack_context =========== 
set (PROJECT_LIBCOPP_STACK_CONTEXT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}")
set (PROJECT_LIBCOPP_STACK_ALLOC_SRC_DIR "${PROJECT_LIBCOPP_STACK_CONTEXT_SRC_DIR}/allocator")

# ========== stack information ==========
aux_source_directory("${PROJECT_LIBCOPP_STACK_CONTEXT_SRC_DIR}" SRC_LIST)
list(APPEND COPP_SRC_LIST ${SRC_LIST})

# ========== stack allocator ==========
list(APPEND COPP_SRC_LIST "${PROJECT_LIBCOPP_STACK_ALLOC_SRC_DIR}/stack_allocator_memory.cpp")
list(APPEND COPP_SRC_LIST "${PROJECT_LIBCOPP_STACK_ALLOC_SRC_DIR}/stack_allocator_malloc.cpp")

include(CheckIncludeFileCXX)
include(CheckIncludeFiles)

CHECK_INCLUDE_FILE_CXX(windows.h PROJECT_LIBCOPP_STACK_ALLOC_WINDOWS)
if(PROJECT_LIBCOPP_STACK_ALLOC_WINDOWS)
	EchoWithColor(COLOR GREEN "-- stack allocator: enable windows allocator")
	list(APPEND COPP_SRC_LIST "${PROJECT_LIBCOPP_STACK_ALLOC_SRC_DIR}/stack_allocator_windows.cpp")
	list(APPEND COPP_SRC_LIST "${PROJECT_LIBCOPP_STACK_CONTEXT_SRC_DIR}/stack_traits/stack_traits_windows.cpp")
	add_compiler_define(COPP_MACRO_SYS_WIN=1)
endif()

CHECK_INCLUDE_FILES("sys/resource.h;sys/time.h;unistd.h;sys/types.h;fcntl.h;sys/mman.h" PROJECT_LIBCOPP_STACK_ALLOC_POSIX)

if(PROJECT_LIBCOPP_STACK_ALLOC_POSIX)
	EchoWithColor(COLOR GREEN "-- stack allocator: enable posix allocator")
	list(APPEND COPP_SRC_LIST "${PROJECT_LIBCOPP_STACK_ALLOC_SRC_DIR}/stack_allocator_posix.cpp")
	list(APPEND COPP_SRC_LIST "${PROJECT_LIBCOPP_STACK_CONTEXT_SRC_DIR}/stack_traits/stack_traits_posix.cpp")
	add_compiler_define(COPP_MACRO_SYS_POSIX=1)
endif()

if(LIBCOPP_ENABLE_SEGMENTED_STACKS)
	EchoWithColor(COLOR GREEN "-- gcc ${CMAKE_CXX_COMPILER_VERSION} found in unix like system.")
	EchoWithColor(COLOR GREEN "-- stack allocator: enable segment allocator, add -fsplit-stack to all program use this library")
	list(APPEND COPP_SRC_LIST "${PROJECT_LIBCOPP_STACK_ALLOC_SRC_DIR}/stack_allocator_split_segment.cpp")
endif()

if (COPP_MACRO_USE_VALGRIND OR LIBCOPP_ENABLE_VALGRIND)
    EchoWithColor(COLOR GREEN "-- gcc enable valgrind.")
	add_definitions(-DCOPP_MACRO_USE_VALGRIND)
endif()
