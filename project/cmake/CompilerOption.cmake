# 默认配置选项
#####################################################################

if (CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Available Build Type: ${CMAKE_CONFIGURATION_TYPES}")
else()
	message(STATUS "Available Build Type: Unknown")
endif()

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
	#set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

# 设置公共编译选项
set(ALL_FLAGS_IN_ONE_COMMON "")
set(C_FLAGS_IN_ONE_COMMON "")
set(CXX_FLAGS_IN_ONE_COMMON "")

# 编译器选项 (仅做了GCC、VC和Clang兼容)
if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_definitions(-Wall -Werror)

    # gcc 4.9 编译输出颜色支持
    if ( CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.9.0" OR CMAKE_CXX_COMPILER_VERSION  VERSION_EQUAL "4.9.0" )
            add_definitions(-fdiagnostics-color=auto)
    endif()
    # 检测GCC版本大于等于4.8时，默认-Wno-unused-local-typedefs (普片用于type_traits，故而关闭该警告)
    if ( CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.8.0" OR CMAKE_CXX_COMPILER_VERSION  VERSION_EQUAL "4.8.0" )
            add_definitions(-Wno-unused-local-typedefs)
            message(STATUS "GCC Version ${CMAKE_CXX_COMPILER_VERSION} Found, -Wno-unused-local-typedefs added.")
    endif()
    
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "5.0.0" OR CMAKE_CXX_COMPILER_VERSION  VERSION_EQUAL "5.0.0" )
        set(CMAKE_C_STANDARD 11)
        set(CMAKE_CXX_STANDARD 14)
        message(STATUS "GCC Version ${CMAKE_CXX_COMPILER_VERSION} , using -std=c11/c++14.")
    elseif ( CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.7.0" OR CMAKE_CXX_COMPILER_VERSION  VERSION_EQUAL "4.7.0" )
        set(CMAKE_C_STANDARD 11)
        set(CMAKE_CXX_STANDARD 11)
        message(STATUS "GCC Version ${CMAKE_CXX_COMPILER_VERSION} , using -std=c11/c++11.")
    elseif( CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.4.0" OR CMAKE_CXX_COMPILER_VERSION  VERSION_EQUAL "4.4.0" )
        set(CXX_FLAGS_IN_ONE_COMMON "${C_FLAGS_IN_ONE_COMMON} -std=c++0x")
        message(STATUS "GCC Version ${CMAKE_CXX_COMPILER_VERSION} , using -std=c++0x.")
    endif()

elseif( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_definitions(-Wall -Werror -fPIC)
    # 苹果系统会误判，不过问题不大，反正也是用最高的标准
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "3.4" OR CMAKE_CXX_COMPILER_VERSION  VERSION_EQUAL "3.4" )
        set(CMAKE_C_STANDARD 11)
        set(CMAKE_CXX_STANDARD 14)
        message(STATUS "Clang Version ${CMAKE_CXX_COMPILER_VERSION} , using -std=c11/c++14.")
    elseif ( CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "3.3" OR CMAKE_CXX_COMPILER_VERSION  VERSION_EQUAL "3.3" )
        set(CMAKE_C_STANDARD 11)
        set(CMAKE_CXX_STANDARD 11)
        message(STATUS "Clang Version ${CMAKE_CXX_COMPILER_VERSION} , using -std=c11/c++11.")
    endif()
endif()

# 配置公共编译选项
if ( NOT MSVC )
        set(ALL_FLAGS_IN_ONE_DEBUG "-ggdb -O0 ${ALL_FLAGS_IN_ONE_COMMON}")
        set(ALL_FLAGS_IN_ONE_RELEASE "${ALL_FLAGS_IN_ONE_COMMON}")
        set(ALL_FLAGS_IN_ONE_RELWITHDEBINFO "-ggdb ${ALL_FLAGS_IN_ONE_COMMON}")
        set(ALL_FLAGS_IN_ONE_MINSIZEREL "${ALL_FLAGS_IN_ONE_COMMON}")
else()
        set(ALL_FLAGS_IN_ONE_DEBUG "/Od /MDd ${ALL_FLAGS_IN_ONE_COMMON}")
        set(ALL_FLAGS_IN_ONE_RELEASE "/O2 /MD ${ALL_FLAGS_IN_ONE_COMMON} /D NDEBUG")
        set(ALL_FLAGS_IN_ONE_RELWITHDEBINFO "/O2 /MDd ${ALL_FLAGS_IN_ONE_COMMON}")
        set(ALL_FLAGS_IN_ONE_MINSIZEREL "/Ox /MD ${ALL_FLAGS_IN_ONE_COMMON} /D NDEBUG")

        #add_definitions(/D SEED_WITH_BOOST_HPP /D SEED_ENABLE_BOOST_FOREACH /D SEED_ENABLE_BOOST_STATIC_ASSERT)
endif()

# 设置实际的编译选项
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${ALL_FLAGS_IN_ONE_DEBUG} ${CXX_FLAGS_IN_ONE_COMMON}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${ALL_FLAGS_IN_ONE_RELEASE} ${CXX_FLAGS_IN_ONE_COMMON}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${ALL_FLAGS_IN_ONE_RELWITHDEBINFO} ${CXX_FLAGS_IN_ONE_COMMON}")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} ${ALL_FLAGS_IN_ONE_MINSIZEREL} ${CXX_FLAGS_IN_ONE_COMMON}")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${ALL_FLAGS_IN_ONE_DEBUG} ${C_FLAGS_IN_ONE_COMMON}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${ALL_FLAGS_IN_ONE_RELEASE} ${C_FLAGS_IN_ONE_COMMON}")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${ALL_FLAGS_IN_ONE_RELWITHDEBINFO} ${C_FLAGS_IN_ONE_COMMON}")
set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} ${ALL_FLAGS_IN_ONE_MINSIZEREL} ${C_FLAGS_IN_ONE_COMMON}")

# 库文件的附加参数 -fPIC, 多线程附加参数 -pthread -D_POSIX_MT_

# 功能函数
macro(add_compiler_define)
	foreach(def ${ARGV})
    	if ( NOT MSVC )
            add_definitions(-D${def})
        else()
            add_definitions("/D ${def}")
        endif()
	endforeach()
endmacro(add_compiler_define)
