# =========== libcopp/src - coroutine task =========== 
set (PROJECT_LIBCOTASK_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}")

# ========== build library ==========
file(GLOB_RECURSE SRC_LIST ${PROJECT_LIBCOTASK_SRC_DIR}/*.c ${PROJECT_LIBCOTASK_SRC_DIR}/*.cpp)
list(LENGTH SRC_LIST SRC_LIST_LENGTH)
if( ${SRC_LIST_LENGTH} GREATER 0 )
	list(APPEND COPP_SRC_LIST ${SRC_LIST})
endif()
