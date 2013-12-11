# =========== libcopp/src - stack_context =========== 
set (PROJECT_LIBCOPP_STACK_CONTEXT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}")
set (PROJECT_LIBCOPP_STACK_ALLOC_SRC_DIR "${PROJECT_LIBCOPP_STACK_CONTEXT_SRC_DIR}/allocator")

# ========== stack information ==========
aux_source_directory("${PROJECT_LIBCOPP_STACK_CONTEXT_SRC_DIR}" SRC_LIST)
list(APPEND COPP_SRC_LIST ${SRC_LIST})

# ========== stack allocator ==========
list(APPEND COPP_SRC_LIST "${PROJECT_LIBCOPP_STACK_ALLOC_SRC_DIR}/stack_allocator_memory.cpp")
if(WIN32)
	EchoWithColor(COLOR GREEN "-- stack allocator: enable windows allocator")
	list(APPEND COPP_SRC_LIST "${PROJECT_LIBCOPP_STACK_ALLOC_SRC_DIR}/stack_allocator_windows.cpp")
endif()

if(NOT WIN32 AND UNIX)
	EchoWithColor(COLOR GREEN "-- stack allocator: enable posix allocator")
	# list(APPEND COPP_SRC_LIST "${PROJECT_LIBCOPP_STACK_ALLOC_SRC_DIR}/stack_allocator_posix.cpp")
endif()

if(NOT WIN32 AND UNIX AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.7.0" OR CMAKE_CXX_COMPILER_VERSION  VERSION_EQUAL "4.7.0")
		EchoWithColor(COLOR GREEN "-- gcc ${CMAKE_CXX_COMPILER_VERSION} found in unix like system.")
		EchoWithColor(COLOR GREEN "-- stack allocator: enable segment allocator, add -DLIBCOPP_USE_GCC_SEGMENTED_STACKS")
		add_definitions(-DLIBCOPP_USE_GCC_SEGMENTED_STACKS)
		# list(APPEND COPP_SRC_LIST "${PROJECT_LIBCOPP_STACK_ALLOC_SRC_DIR}/stack_allocator_segmented.cpp")
	endif()
endif()
