# =========== libcopp/src ===========
set(PROJECT_LIBCOPP_INC_DIR "${PROJECT_LIBCOPP_ROOT_INC_DIR}/libcopp")
set(PROJECT_LIBCOPP_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(PROJECT_LIBCOPP_LIB_LINK copp)
set(PROJECT_LIBCOPP_EXPORT_NAME libcopp-target)

# ========== filter sources files ==========
file(GLOB_RECURSE COPP_SRC_LIST ${PROJECT_LIBCOPP_INC_DIR}/*.h ${PROJECT_LIBCOPP_INC_DIR}/*.hpp)
unset(COPP_OBJ_LIST)

include("${PROJECT_LIBCOPP_SRC_DIR}/fcontext/fcontext.src.filter.cmake")
include("${PROJECT_LIBCOPP_SRC_DIR}/stack/stack.src.filter.cmake")
include("${PROJECT_LIBCOPP_SRC_DIR}/utils/utils.src.filter.cmake")
include("${PROJECT_LIBCOPP_SRC_DIR}/coroutine/coroutine.src.filter.cmake")
