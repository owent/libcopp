# platform check
# default to x86 platform.  We'll check for X64 in a bit

if(NOT DEFINED __FIND_PLATFORM_LOADED)
  set(__FIND_PLATFORM_LOADED 1)
  set(PLATFORM_SUFFIX "")

  # This definition is necessary to work around a bug with Intellisense described here: http://tinyurl.com/2cb428.  Syntax highlighting is
  # important for proper debugger functionality.

  if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    # MESSAGE(STATUS "Detected 64-bit platform.")
    if(WIN32)
      add_definitions("-D_WIN64")
    endif()
    set(PLATFORM_SUFFIX "64")
  else()
    # MESSAGE(STATUS "Detected 32-bit platform.")
  endif()
endif()
