# =========== libcopp/src ===========
set(PROJECT_LIBCOTASK_INC_DIR "${PROJECT_LIBCOPP_ROOT_INC_DIR}/libcotask")
set(PROJECT_LIBCOTASK_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(PROJECT_LIBCOTASK_LIB_LINK cotask)
set(PROJECT_LIBCOTASK_EXPORT_NAME libcotask-target)

include_macro_recurse(FILTER "*.src.filter.cmake" "${PROJECT_LIBCOTASK_SRC_DIR}")
