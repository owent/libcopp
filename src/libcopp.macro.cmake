# =========== libcopp/src =========== 
set (PROJECT_LIBCOPP_ROOT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}")
unset (PROJECT_LIBCOPP_DEP_INC_DIR)

include("${PROJECT_LIBCOPP_ROOT_SRC_DIR}/libcopp/libcopp.lib.cmake")

# ========= libcotask =========  
if (LIBCOTASK_ENABLE)
    include("${PROJECT_LIBCOPP_ROOT_SRC_DIR}/libcotask/libcotask.lib.cmake")
endif()