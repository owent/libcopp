# =========== libcopp/src =========== 
set (PROJECT_ROOT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}")

include("${PROJECT_ROOT_SRC_DIR}/libcopp/libcopp.lib.cmake")

# ========= libcotask =========  
if (LIBCOTASK_ENABLE)
    include("${PROJECT_ROOT_SRC_DIR}/libcotask/libcotask.lib.cmake")
endif()