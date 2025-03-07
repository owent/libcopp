include_guard(GLOBAL)

# ASAN_OPTIONS=sleep_before_dying=3:abort_on_error=1:halt_on_error=1:disable_coredump=0:disable_core=0:unmap_shadow_on_exit=1:detect_leaks=1:atexit=1:log_path=process_name.asan.log
# ASAN_OPTIONS=set windows_hook_rtl_allocators=true

unset(PROJECT_SANTIZER_COMPILE_FLAGS)
unset(PROJECT_SANTIZER_RUNTIME_LINK_FLAGS)

if(PROJECT_SANTIZER_USE_ADDRESS
   OR PROJECT_SANTIZER_USE_THREAD
   OR PROJECT_SANTIZER_USE_LEAK
   OR PROJECT_SANTIZER_USE_UNDEFINED
   OR PROJECT_SANTIZER_USE_HWADDRESS)
  include(CheckCXXSourceCompiles)
  include(CheckIncludeFile)
  include(CMakePushCheckState)

  cmake_push_check_state()

  find_package(Threads)
  if(CMAKE_USE_PTHREADS_INIT)
    list(APPEND CMAKE_REQUIRED_LIBRARIES Threads::Threads)
    if(CMAKE_USE_PTHREADS_INIT)
      set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -pthread")
    endif()
  endif()
endif()

if(PROJECT_SANTIZER_USE_ADDRESS)
  # 低版本的编译器对coredump不友好，所以还是禁用掉吧
  if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "7.0.0")
     OR CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
    list(APPEND CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=address")
    list(APPEND CMAKE_REQUIRED_LIBRARIES ${COMPILER_OPTION_EXTERN_CXX_LIBS})
    project_build_tools_append_space_flags_to_var(CMAKE_REQUIRED_FLAGS "-fsanitize=address")

    unset(PROJECT_SANTIZER_TEST_ADDRESS CACHE)
    check_cxx_source_compiles("#include<iostream>
        int main() { return 0; }" PROJECT_SANTIZER_TEST_ADDRESS)
    if(PROJECT_SANTIZER_TEST_ADDRESS)
      list(APPEND PROJECT_SANTIZER_RUNTIME_LINK_FLAGS "-fsanitize=address")
      set(PROJECT_SANTIZER_COMPILE_FLAGS "-fsanitize=address")

      # Check -fsanitize-recover=address
      set(CMAKE_REQUIRED_FLAGS_BACKUP "${CMAKE_REQUIRED_FLAGS}")
      set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -fsanitize-recover=address")
      unset(PROJECT_SANTIZER_TEST_RECOVER_ADDRESS CACHE)
      check_cxx_source_compiles("#include<iostream>
            int main() { return 0; }" PROJECT_SANTIZER_TEST_RECOVER_ADDRESS)
      if(PROJECT_SANTIZER_TEST_RECOVER_ADDRESS)
        set(PROJECT_SANTIZER_COMPILE_FLAGS "${PROJECT_SANTIZER_COMPILE_FLAGS} -fsanitize-recover=address")
      else()
        set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS_BACKUP}")
      endif()
      unset(CMAKE_REQUIRED_FLAGS_BACKUP)
    else()
      echowithcolor(COLOR RED "-- Check -fsanitize=address failed")
      message(FATAL_ERROR "Current compiler do not support a modern version of AddressSsanitizer")
    endif()
    unset(PROJECT_SANTIZER_TEST_ADDRESS CACHE)
    unset(PROJECT_SANTIZER_TEST_RECOVER_ADDRESS CACHE)
    unset(PROJECT_SANTIZER_TEST_STATIC_LIBASAN CACHE)
  elseif(MSVC)
    if((MSVC_VERSION GREATER_EQUAL 1924) AND CMAKE_SIZEOF_VOID_P MATCHES 4)
      # https://devblogs.microsoft.com/cppblog/addresssanitizer-asan-for-windows-with-msvc/ Require to link asan library
      # with /wholearchive:<library to link>
      include(CheckCXXSourceCompiles)
      # list(APPEND CMAKE_REQUIRED_LIBRARIES ${COMPILER_OPTION_EXTERN_CXX_LIBS} "clang_rt.asan_cxx-i386") # For /MT,/MTd
      # list(APPEND CMAKE_REQUIRED_LIBRARIES ${COMPILER_OPTION_EXTERN_CXX_LIBS} "clang_rt.asan_dll_thunk-i386") # For
      # /MT,/MTd list(APPEND CMAKE_REQUIRED_LIBRARIES ${COMPILER_OPTION_EXTERN_CXX_LIBS} "clang_rt.asan_dynamic-i386") #
      # For /MD,/MDd list(APPEND CMAKE_REQUIRED_LIBRARIES ${COMPILER_OPTION_EXTERN_CXX_LIBS}
      # "clang_rt.asan_dynamic_runtime_thunk-i386") # For /MD,/MDd
      project_build_tools_append_space_flags_to_var(CMAKE_REQUIRED_FLAGS "/fsanitize=address")

      unset(PROJECT_SANTIZER_TEST_ADDRESS CACHE)
      check_cxx_source_compiles("#include<iostream>
            int main() { return 0; }" PROJECT_SANTIZER_TEST_ADDRESS)
      if(PROJECT_SANTIZER_TEST_ADDRESS)
        set(PROJECT_SANTIZER_COMPILE_FLAGS "/fsanitize=address")
        echowithcolor(COLOR RED "-- Check /fsanitize=address failed")
      endif()

      unset(PROJECT_SANTIZER_TEST_ADDRESS CACHE)
    else()
      message(FATAL_ERROR "Current compiler do not support a modern version of AddressSanitizer")
    endif()
  else()
    message(FATAL_ERROR "Current compiler do not support a modern version of AddressSanitizer")
  endif()
elseif(PROJECT_SANTIZER_USE_THREAD)
  # 低版本的编译器对coredump不友好，所以还是禁用掉吧
  if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "7.0.0")
     OR CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
    list(APPEND CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=thread")
    list(APPEND CMAKE_REQUIRED_LIBRARIES ${COMPILER_OPTION_EXTERN_CXX_LIBS})
    project_build_tools_append_space_flags_to_var(CMAKE_REQUIRED_FLAGS "-fsanitize=thread -DTHREAD_SANITIZER")

    unset(PROJECT_SANTIZER_TEST_THREAD CACHE)
    check_cxx_source_compiles("#include<iostream>
        int main() { return 0; }" PROJECT_SANTIZER_TEST_THREAD)
    if(PROJECT_SANTIZER_TEST_THREAD)
      list(APPEND PROJECT_SANTIZER_RUNTIME_LINK_FLAGS "-fsanitize=thread")
      set(PROJECT_SANTIZER_COMPILE_FLAGS "-fsanitize=thread -DTHREAD_SANITIZER")
      if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(PROJECT_SANTIZER_COMPILE_FLAGS "${PROJECT_SANTIZER_COMPILE_FLAGS} -Wno-error=tsan")
      endif()
    else()
      echowithcolor(COLOR RED "-- Check -fsanitize=thread failed")
      message(FATAL_ERROR "Current compiler do not support a modern version of ThreadSanitizer")
    endif()
    unset(PROJECT_SANTIZER_TEST_THREAD CACHE)
    unset(PROJECT_SANTIZER_TEST_STATIC_LIBTSAN CACHE)
  else()
    message(FATAL_ERROR "Current compiler do not support a modern version of ThreadSanitizer")
  endif()
elseif(PROJECT_SANTIZER_USE_LEAK)
  if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "7.0.0")
     OR CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
    list(APPEND CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=leak")
    list(APPEND CMAKE_REQUIRED_LIBRARIES ${COMPILER_OPTION_EXTERN_CXX_LIBS})
    project_build_tools_append_space_flags_to_var(CMAKE_REQUIRED_FLAGS "-fsanitize=leak")

    unset(PROJECT_SANTIZER_TEST_LEAK CACHE)
    check_cxx_source_compiles("#include<iostream>
        int main() { return 0; }" PROJECT_SANTIZER_TEST_LEAK)
    if(PROJECT_SANTIZER_TEST_LEAK)
      list(APPEND PROJECT_SANTIZER_RUNTIME_LINK_FLAGS "-fsanitize=leak")
      set(PROJECT_SANTIZER_COMPILE_FLAGS "-fsanitize=leak")
    else()
      echowithcolor(COLOR RED "-- Check -fsanitize=leak failed")
      message(FATAL_ERROR "Current compiler do not support a modern version of LeakSanitizer")
    endif()
    unset(PROJECT_SANTIZER_TEST_LEAK CACHE)
    unset(PROJECT_SANTIZER_TEST_STATIC_LIBLSAN CACHE)
  else()
    message(FATAL_ERROR "Current compiler do not support a modern version of LeakSanitizer")
  endif()
elseif(PROJECT_SANTIZER_USE_UNDEFINED)
  # 低版本的编译器对coredump不友好，所以还是禁用掉吧
  if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "7.0.0")
     OR CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
    list(APPEND CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=undefined")
    list(APPEND CMAKE_REQUIRED_LIBRARIES ${COMPILER_OPTION_EXTERN_CXX_LIBS})
    project_build_tools_append_space_flags_to_var(CMAKE_REQUIRED_FLAGS "-fsanitize=undefined")

    unset(PROJECT_SANTIZER_TEST_UNDEFINED CACHE)
    check_cxx_source_compiles("#include<iostream>
        int main() { return 0; }" PROJECT_SANTIZER_TEST_UNDEFINED)
    if(PROJECT_SANTIZER_TEST_UNDEFINED)
      list(APPEND PROJECT_SANTIZER_RUNTIME_LINK_FLAGS "-fsanitize=undefined")
      set(PROJECT_SANTIZER_COMPILE_FLAGS "-fsanitize=undefined")
    else()
      echowithcolor(COLOR RED "-- Check -fsanitize=undefined failed")
      message(FATAL_ERROR "Current compiler do not support a modern version of UndefinedSanitizer")
    endif()
    unset(PROJECT_SANTIZER_TEST_UNDEFINED CACHE)
    unset(PROJECT_SANTIZER_TEST_STATIC_LIBUBSAN CACHE)
  else()
    message(FATAL_ERROR "Current compiler do not support a modern version of UndefinedSanitizer")
  endif()
elseif(PROJECT_SANTIZER_USE_HWADDRESS)
  # 低版本的编译器对coredump不友好，所以还是禁用掉吧
  if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "7.0.0")
     OR CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
    list(APPEND CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=hwaddress")
    list(APPEND CMAKE_REQUIRED_LIBRARIES ${COMPILER_OPTION_EXTERN_CXX_LIBS})
    project_build_tools_append_space_flags_to_var(CMAKE_REQUIRED_FLAGS "-fsanitize=hwaddress")

    unset(PROJECT_SANTIZER_TEST_HWADDRESS CACHE)
    check_cxx_source_compiles("#include<iostream>
        int main() { return 0; }" PROJECT_SANTIZER_TEST_HWADDRESS)
    if(PROJECT_SANTIZER_TEST_HWADDRESS)
      list(APPEND PROJECT_SANTIZER_RUNTIME_LINK_FLAGS "-fsanitize=hwaddress")
      set(PROJECT_SANTIZER_COMPILE_FLAGS "-fsanitize=hwaddress")
    else()
      echowithcolor(COLOR RED "-- Check -fsanitize=hwaddress failed")
      message(FATAL_ERROR "Current compiler do not support a modern version of HardwareAddressSanitizer")
    endif()
    unset(PROJECT_SANTIZER_TEST_HWADDRESS CACHE)
    unset(PROJECT_SANTIZER_TEST_STATIC_LIBHWASAN CACHE)
  else()
    message(FATAL_ERROR "Current compiler do not support a modern version of HardwareAddressSanitizer")
  endif()
endif()

if(PROJECT_SANTIZER_USE_ADDRESS
   OR PROJECT_SANTIZER_USE_THREAD
   OR PROJECT_SANTIZER_USE_LEAK
   OR PROJECT_SANTIZER_USE_UNDEFINED
   OR PROJECT_SANTIZER_USE_HWADDRESS)
  cmake_pop_check_state()
endif()

if(PROJECT_SANTIZER_COMPILE_FLAGS)
  add_compiler_flags_to_inherit_var(CMAKE_C_FLAGS "${PROJECT_SANTIZER_COMPILE_FLAGS}")
  add_compiler_flags_to_inherit_var(CMAKE_CXX_FLAGS "${PROJECT_SANTIZER_COMPILE_FLAGS}")
  echowithcolor(COLOR YELLOW "-- Enable santizer flags: ${PROJECT_SANTIZER_COMPILE_FLAGS}")

  if(PROJECT_SANTIZER_USE_ADDRESS)
    check_include_file("sanitizer/asan_interface.h" SERVER_FRAME_ENABLE_SANITIZER_ASAN_INTERFACE_TEST)
    if(SERVER_FRAME_ENABLE_SANITIZER_ASAN_INTERFACE_TEST)
      set(SERVER_FRAME_ENABLE_SANITIZER_ASAN_INTERFACE 1)
    endif()
    unset(SERVER_FRAME_ENABLE_SANITIZER_ASAN_INTERFACE_TEST CACHE)

    set(PROJECT_WITH_SANTIZER_NAME "address")
  elseif(PROJECT_SANTIZER_USE_THREAD)
    check_include_file("sanitizer/tsan_interface.h" SERVER_FRAME_ENABLE_SANITIZER_TSAN_INTERFACE_TEST)
    if(SERVER_FRAME_ENABLE_SANITIZER_TSAN_INTERFACE_TEST)
      set(SERVER_FRAME_ENABLE_SANITIZER_TSAN_INTERFACE 1)
    endif()
    unset(SERVER_FRAME_ENABLE_SANITIZER_TSAN_INTERFACE_TEST CACHE)

    set(PROJECT_WITH_SANTIZER_NAME "thread")
  elseif(PROJECT_SANTIZER_USE_LEAK)
    check_include_file("sanitizer/lsan_interface.h" SERVER_FRAME_ENABLE_SANITIZER_LSAN_INTERFACE_TEST)
    if(SERVER_FRAME_ENABLE_SANITIZER_LSAN_INTERFACE_TEST)
      set(SERVER_FRAME_ENABLE_SANITIZER_LSAN_INTERFACE 1)
    endif()
    unset(SERVER_FRAME_ENABLE_SANITIZER_LSAN_INTERFACE_TEST CACHE)

    set(PROJECT_WITH_SANTIZER_NAME "leak")
  elseif(PROJECT_SANTIZER_USE_UNDEFINED)
    check_include_file("sanitizer/ubsan_interface.h" SERVER_FRAME_ENABLE_SANITIZER_UBSAN_INTERFACE_TEST)
    if(SERVER_FRAME_ENABLE_SANITIZER_UBSAN_INTERFACE_TEST)
      set(SERVER_FRAME_ENABLE_SANITIZER_UBSAN_INTERFACE 1)
    endif()
    unset(SERVER_FRAME_ENABLE_SANITIZER_UBSAN_INTERFACE_TEST CACHE)

    set(PROJECT_WITH_SANTIZER_NAME "undefined")
  elseif(PROJECT_SANTIZER_USE_HWADDRESS)
    check_include_file("sanitizer/hwasan_interface.h" SERVER_FRAME_ENABLE_SANITIZER_HWASAN_INTERFACE_TEST)
    if(SERVER_FRAME_ENABLE_SANITIZER_HWASAN_INTERFACE_TEST)
      set(SERVER_FRAME_ENABLE_SANITIZER_HWASAN_INTERFACE 1)
    endif()
    unset(SERVER_FRAME_ENABLE_SANITIZER_HWASAN_INTERFACE_TEST CACHE)

    set(PROJECT_WITH_SANTIZER_NAME "hwaddress")
  endif()
endif()
if(PROJECT_SANTIZER_RUNTIME_LINK_FLAGS)
  add_linker_flags_for_runtime_inherit(${PROJECT_SANTIZER_RUNTIME_LINK_FLAGS})
  echowithcolor(COLOR YELLOW "-- Enable santizer linker flags: ${PROJECT_SANTIZER_RUNTIME_LINK_FLAGS}")
  # Check sanitizer link type
  if(PROJECT_WITH_SANTIZER_RUNTIME AND PROJECT_WITH_SANTIZER_RUNTIME STREQUAL "static")
    set(PROJECT_SANTIZER_USE_STATIC TRUE)
  elseif(PROJECT_WITH_SANTIZER_RUNTIME AND PROJECT_WITH_SANTIZER_RUNTIME STREQUAL "shared")
    set(PROJECT_SANTIZER_USE_STATIC FALSE)
  else()
    project_build_tools_sanitizer_use_static(PROJECT_SANTIZER_USE_STATIC)
  endif()
  if(PROJECT_SANTIZER_USE_STATIC)
    set(PROJECT_WITH_SANTIZER_RUNTIME "static")
    project_build_tools_sanitizer_try_get_static_link(PROJECT_SANTIZER_TEST_LINK_TYPE
                                                      ${PROJECT_SANTIZER_RUNTIME_LINK_FLAGS})
  else()
    set(PROJECT_WITH_SANTIZER_RUNTIME "shared")
    project_build_tools_sanitizer_try_get_shared_link(PROJECT_SANTIZER_TEST_LINK_TYPE
                                                      ${PROJECT_SANTIZER_RUNTIME_LINK_FLAGS})
  endif()
  if(PROJECT_SANTIZER_TEST_LINK_TYPE)
    add_compiler_flags_to_inherit_var_unique(CMAKE_EXE_LINKER_FLAGS ${PROJECT_SANTIZER_TEST_LINK_TYPE})
    echowithcolor(COLOR YELLOW "-- Enable santizer link type for executable: ${PROJECT_SANTIZER_TEST_LINK_TYPE}")
  endif()
endif()

unset(PROJECT_SANTIZER_USE_STATIC)
unset(PROJECT_SANTIZER_COMPILE_FLAGS)
unset(PROJECT_SANTIZER_RUNTIME_LINK_FLAGS)
