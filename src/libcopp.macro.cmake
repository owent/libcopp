# =========== libcopp/src =========== 
set (PROJECT_LIBCOPP_SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set (PROJECT_LIBCOPP_LIB_LINK copp)

# ========== filter sources files ==========
file(GLOB_RECURSE COPP_SRC_LIST ${PROJECT_LIBCOPP_INC_DIR}/*.h ${PROJECT_LIBCOPP_INC_DIR}/*.hpp)
include_macro_recurse(FILTER "*.src.filter.cmake" "${PROJECT_LIBCOPP_SRC_DIR}")
