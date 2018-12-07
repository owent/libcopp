# =========== libcopp/src ===========
set (PROJECT_LIBCOTASK_INC_DIR "${PROJECT_LIBCOPP_ROOT_INC_DIR}/libcotask") 
set (PROJECT_LIBCOTASK_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}")
set (PROJECT_LIBCOTASK_LIB_LINK cotask)

include_macro_recurse(FILTER "*.src.filter.cmake" "${PROJECT_LIBCOTASK_SRC_DIR}")
