# =========== libcopp/src - fcontext - detect =========== 
set (PROJECT_LIBCOPP_FCONTEXT_DETECT_DIR "${PROJECT_LIBCOPP_FCONTEXT_SRC_DIR}/detect")

# ========== set os platform ==========
if (NOT LIBCOPP_FCONTEXT_OS_PLATFORM)
	if ("${PLATFORM_SUFFIX}" STREQUAL "64")
		set(LIBCOPP_FCONTEXT_OS_PLATFORM "x86_64")
	else()
		set(LIBCOPP_FCONTEXT_OS_PLATFORM "i386")
	endif()
endif()

# ========== set abi ==========
if(NOT LIBCOPP_FCONTEXT_ABI)
	set(LIBCOPP_FCONTEXT_ABI "sysv")
	
	if( "${LIBCOPP_FCONTEXT_OS_PLATFORM}" STREQUAL "arm" )
		set(LIBCOPP_FCONTEXT_ABI "aapcs")
	elseif( "${LIBCOPP_FCONTEXT_OS_PLATFORM}" STREQUAL "mips" )
		set(LIBCOPP_FCONTEXT_ABI "o32")
	elseif (WIN32 OR CYGWIN)
		set(LIBCOPP_FCONTEXT_ABI "ms")
	endif()
endif()

# ========== set binary format ==========
if(NOT LIBCOPP_FCONTEXT_BIN_FORMAT)
	set(LIBCOPP_FCONTEXT_BIN_FORMAT "elf")
	
	if(WIN32 OR CYGWIN)
		if ("${LIBCOPP_FCONTEXT_OS_PLATFORM}" STREQUAL "arm" )
			set(LIBCOPP_FCONTEXT_BIN_FORMAT "pe_armasm")
		else()
			set(LIBCOPP_FCONTEXT_BIN_FORMAT "pe_masm")
		endif()
	elseif(APPLE)
		set(LIBCOPP_FCONTEXT_BIN_FORMAT "macho")
	endif()
endif()

# ========== set as tool ==========
if(NOT LIBCOPP_FCONTEXT_AS_TOOL)
	set(LIBCOPP_FCONTEXT_AS_TOOL "as")
	
	if(WIN32 OR CYGWIN)
		if ("${LIBCOPP_FCONTEXT_OS_PLATFORM}" STREQUAL "arm" )
			set(LIBCOPP_FCONTEXT_AS_TOOL "armasm")
		elseif( "${PLATFORM_SUFFIX}" STREQUAL "64" )
			set(LIBCOPP_FCONTEXT_AS_TOOL "ml64")
		else()
			set(LIBCOPP_FCONTEXT_AS_TOOL "ml")
		endif()
	endif()
endif()