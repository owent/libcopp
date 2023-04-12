# =========== libcopp/src - fcontext - detect ===========
set(PROJECT_LIBCOPP_FCONTEXT_DETECT_DIR "${PROJECT_LIBCOPP_FCONTEXT_SRC_DIR}/detect")

# ========== set os platform ==========
# LIBCOPP_FCONTEXT_OS_PLATFORM can be set to arm/arm64/i386/x86_64/combined/mips/ppc32/ppc64/sparc/sparc64
if(NOT LIBCOPP_FCONTEXT_OS_PLATFORM)
  if(NOT CMAKE_SYSTEM_PROCESSOR)
    set(CMAKE_SYSTEM_PROCESSOR ${CMAKE_HOST_SYSTEM_PROCESSOR})
  endif()
  if(ANDROID)
    if(CMAKE_ANDROID_ARCH_ABI)
      string(TOLOWER ${CMAKE_ANDROID_ARCH_ABI} LIBCOPP_FCONTEXT_OS_PLATFORM)
    elseif(ANDROID_ABI)
      string(TOLOWER ${ANDROID_ABI} LIBCOPP_FCONTEXT_OS_PLATFORM)
    endif()
  elseif(CMAKE_OSX_ARCHITECTURES)
    string(TOLOWER ${CMAKE_OSX_ARCHITECTURES} LIBCOPP_FCONTEXT_OS_PLATFORM)
  endif()
  if(NOT LIBCOPP_FCONTEXT_OS_PLATFORM)
    string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} LIBCOPP_FCONTEXT_OS_PLATFORM)
  endif()
  if(LIBCOPP_FCONTEXT_OS_PLATFORM MATCHES "unknown|i386|i686|x86_64|amd64")
    if(CMAKE_SIZEOF_VOID_P MATCHES 8)
      set(CMAKE_SYSTEM_PROCESSOR "x86_64")
      set(LIBCOPP_FCONTEXT_OS_PLATFORM "x86_64")
    else()
      set(CMAKE_SYSTEM_PROCESSOR "i386")
      set(LIBCOPP_FCONTEXT_OS_PLATFORM "i386")
    endif()
  elseif(LIBCOPP_FCONTEXT_OS_PLATFORM MATCHES "^arm(v5|v7|v7s|eabi)")
    set(LIBCOPP_FCONTEXT_OS_PLATFORM "arm")
  elseif(LIBCOPP_FCONTEXT_OS_PLATFORM MATCHES "^arm64" OR LIBCOPP_FCONTEXT_OS_PLATFORM MATCHES "^aarch64")
    set(LIBCOPP_FCONTEXT_OS_PLATFORM "arm64")
  endif()

  if(LIBCOPP_FCONTEXT_OS_PLATFORM STREQUAL "unknown")
    echowithcolor(
      COLOR
      RED
      "Can not detect the processor, please add -DLIBCOPP_FCONTEXT_OS_PLATFORM=arm/arm64/i386/x86_64/combined/mips32/mips64/ppc32/ppc64/loongarch64 to help us to find target processor"
    )
    message(FATAL_ERROR "-DLIBCOPP_FCONTEXT_OS_PLATFORM is required")
  endif()

endif()

# patch for old -DLIBCOPP_FCONTEXT_OS_PLATFORM=mips to -DLIBCOPP_FCONTEXT_OS_PLATFORM=mips32
if(LIBCOPP_FCONTEXT_OS_PLATFORM STREQUAL "mips")
  set(LIBCOPP_FCONTEXT_OS_PLATFORM "mips32")
endif()

# ========== set abi ==========
if(NOT LIBCOPP_FCONTEXT_ABI)
  set(LIBCOPP_FCONTEXT_ABI "sysv")

  if(LIBCOPP_FCONTEXT_OS_PLATFORM STREQUAL "arm" OR LIBCOPP_FCONTEXT_OS_PLATFORM STREQUAL "arm64")
    set(LIBCOPP_FCONTEXT_ABI "aapcs")
  elseif(LIBCOPP_FCONTEXT_OS_PLATFORM STREQUAL "mips32")
    set(LIBCOPP_FCONTEXT_ABI "o32")
  elseif(LIBCOPP_FCONTEXT_OS_PLATFORM STREQUAL "mips64")
    set(LIBCOPP_FCONTEXT_ABI "n64")
  elseif(
    WIN32
    OR WINCE
    OR WINDOWS_PHONE
    OR WINDOWS_STORE
    OR MINGW
    OR CYGWIN)
    set(LIBCOPP_FCONTEXT_ABI "ms")
  endif()
endif()

# ========== set binary format ==========
# LIBCOPP_FCONTEXT_ABI can be set to elf/macho/pe/xcoff
if(NOT LIBCOPP_FCONTEXT_BIN_FORMAT)
  set(LIBCOPP_FCONTEXT_BIN_FORMAT "elf")

  if(WIN32
     OR WINCE
     OR WINDOWS_PHONE
     OR WINDOWS_STORE
     OR MINGW
     OR CYGWIN)
    set(LIBCOPP_FCONTEXT_BIN_FORMAT "pe")
  elseif(APPLE)
    set(LIBCOPP_FCONTEXT_BIN_FORMAT "macho")
  elseif(AIX) # cmake not supported now
    set(LIBCOPP_FCONTEXT_BIN_FORMAT "xcoff")
  endif()
endif()

# ========== set as tool ==========
# LIBCOPP_FCONTEXT_AS_TOOL can be set to gas/armasm/masm
if(NOT LIBCOPP_FCONTEXT_AS_TOOL)
  set(LIBCOPP_FCONTEXT_AS_TOOL "gas")
  set(LIBCOPP_FCONTEXT_AS_OUTPUT_SUFFIX "o")

  if(WIN32 OR MINGW)
    set(LIBCOPP_FCONTEXT_AS_OUTPUT_SUFFIX "obj")
  endif()
  if(MSVC)
    if("${LIBCOPP_FCONTEXT_OS_PLATFORM}" STREQUAL "arm" OR "${LIBCOPP_FCONTEXT_OS_PLATFORM}" STREQUAL "arm64")
      set(LIBCOPP_FCONTEXT_AS_TOOL "armasm")
    else()
      set(LIBCOPP_FCONTEXT_AS_TOOL "masm")
    endif()
  elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" AND LIBCOPP_FCONTEXT_BIN_FORMAT STREQUAL "pe")
    set(LIBCOPP_FCONTEXT_AS_TOOL "clang_gas")
  endif()

endif()
