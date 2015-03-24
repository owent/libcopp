cmake_minimum_required(VERSION 2.8.9)
project(libcopp)

########################################################################
# CMake 模块 (递归包含模块, 带颜色输出模块, 平台检测模块)
set (PROJECT_CMAKE_MODULE_DIR "${CMAKE_CURRENT_LIST_DIR}/project/cmake")
include("${PROJECT_CMAKE_MODULE_DIR}/ProjectBuildOption.cmake")
include("${PROJECT_CMAKE_MODULE_DIR}/IncludeDirectoryRecurse.cmake")
include("${PROJECT_CMAKE_MODULE_DIR}/EchoWithColor.cmake")
include("${PROJECT_CMAKE_MODULE_DIR}/FindPlatform.cmake")
include("${PROJECT_CMAKE_MODULE_DIR}/ProjectTools.cmake")




#####################################################################
# 导入编译器和编译选项配置
include("${PROJECT_CMAKE_MODULE_DIR}/CompilerOption.cmake")
include("${PROJECT_CMAKE_MODULE_DIR}/TargetOption.cmake")
EchoWithColor(COLOR GREEN "-- Build Type: ${CMAKE_BUILD_TYPE}")

########################################################################
# 导入项目配置
## 导入所有 macro 定义
include_macro_recurse("${PROJECT_SOURCE_DIR}")

## 导入所有工程项目
add_project_recurse("${PROJECT_SOURCE_DIR}")

