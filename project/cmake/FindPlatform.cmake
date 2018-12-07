set(PLATFORM_SUFFIX "")

if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    #MESSAGE(STATUS "Detected 64-bit platform.")
    if(WIN32)
        ADD_DEFINITIONS("-D_WIN64")
    endif()
    set(PLATFORM_SUFFIX "64")
endif()
