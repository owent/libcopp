# 默认配置选项
#####################################################################
option(BUILD_SHARED_LIBS "Build shared libraries (DLLs)." OFF)
option(LIBCOPP_ENABLE_SEGMENTED_STACKS "Enable segmented stacks." OFF)

# libcotask 配置选项
option(LIBCOTASK_ENABLE "Enable libcotask." ON)

# 测试配置选项
set(GTEST_ROOT "" CACHE STRING "GTest root directory")
