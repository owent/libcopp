# =========== libcopp/src - fcontext - detect =========== 
set (PROJECT_LIBCOPP_FCONTEXT_DETECT_DIR "${PROJECT_LIBCOPP_FCONTEXT_SRC_DIR}/detect")

# ========== set os platform ==========
# LIBCOPP_FCONTEXT_ABI can be set to arm/arm64/i386/x86_64/combined/mips/ppc32/ppc64/sparc/sparc64
if (NOT LIBCOPP_FCONTEXT_OS_PLATFORM)
    string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} LIBCOPP_FCONTEXT_OS_PLATFORM)
	if("${LIBCOPP_FCONTEXT_OS_PLATFORM}" STREQUAL "amd64")
        set(LIBCOPP_FCONTEXT_OS_PLATFORM "x86_64")
    endif()

	if (CMAKE_SYSTEM_PROCESSOR STREQUAL "unknown")
		if(CMAKE_SIZEOF_VOID_P MATCHES 8)
			set(CMAKE_SYSTEM_PROCESSOR "x86_64") 
			set(LIBCOPP_FCONTEXT_OS_PLATFORM "x86_64")
		else()
			set(CMAKE_SYSTEM_PROCESSOR "i386")
			set(LIBCOPP_FCONTEXT_OS_PLATFORM "i386")
		endif()
		EchoWithColor(COLOR YELLOW "Can not detect the processor, guess it's ${CMAKE_SYSTEM_PROCESSOR}")
		# message(FATAL_ERROR ${CMAKE_SYSTEM_PROCESSOR})
	endif()
	
endif()

# ========== set abi ==========
if(NOT LIBCOPP_FCONTEXT_ABI)
	set(LIBCOPP_FCONTEXT_ABI "sysv")
	
	if( "${LIBCOPP_FCONTEXT_OS_PLATFORM}" STREQUAL "arm" OR "${LIBCOPP_FCONTEXT_OS_PLATFORM}" STREQUAL "arm64")
		set(LIBCOPP_FCONTEXT_ABI "aapcs")
	elseif( "${LIBCOPP_FCONTEXT_OS_PLATFORM}" STREQUAL "mips" )
		set(LIBCOPP_FCONTEXT_ABI "o32")
	elseif (WIN32 OR WINCE OR WINDOWS_PHONE OR WINDOWS_STORE OR MINGW OR CYGWIN)
		set(LIBCOPP_FCONTEXT_ABI "ms")
	endif()
endif()

# ========== set binary format ==========
# LIBCOPP_FCONTEXT_ABI can be set to elf/macho/pe/xcoff
if(NOT LIBCOPP_FCONTEXT_BIN_FORMAT)
	set(LIBCOPP_FCONTEXT_BIN_FORMAT "elf")
	
	if(WIN32 OR WINCE OR WINDOWS_PHONE OR WINDOWS_STORE OR MINGW OR CYGWIN)
		set(LIBCOPP_FCONTEXT_BIN_FORMAT "pe")
	elseif(APPLE)
		set(LIBCOPP_FCONTEXT_BIN_FORMAT "macho")
	elseif(AIX) # cmake not supported now
		set(LIBCOPP_FCONTEXT_BIN_FORMAT "xcoff")
	endif()
endif()

# ========== set as tool ==========
# LIBCOPP_FCONTEXT_AS_TOOL can be set to gas/armasm/masm
# LIBCOPP_FCONTEXT_AS_ACTION
if(NOT LIBCOPP_FCONTEXT_AS_TOOL)
	set(LIBCOPP_FCONTEXT_AS_TOOL "gas")
	
	if(WIN32 AND MSVC)
		if ("${LIBCOPP_FCONTEXT_OS_PLATFORM}" STREQUAL "arm" OR "${LIBCOPP_FCONTEXT_OS_PLATFORM}" STREQUAL "arm64")
			set(LIBCOPP_FCONTEXT_AS_TOOL "armasm")
		else()
			set(LIBCOPP_FCONTEXT_AS_TOOL "masm")
			if( NOT LIBCOPP_FCONTEXT_AS_ACTION AND "${LIBCOPP_FCONTEXT_OS_PLATFORM}" STREQUAL "64" )
				set(LIBCOPP_FCONTEXT_AS_ACTION "64")
			endif()
		endif()
	endif()
endif()