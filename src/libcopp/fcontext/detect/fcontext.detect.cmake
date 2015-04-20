# =========== libcopp/src - fcontext - detect =========== 
set (PROJECT_LIBCOPP_FCONTEXT_DETECT_DIR "${PROJECT_LIBCOPP_FCONTEXT_SRC_DIR}/detect")

# ========== set os platform ==========
# LIBCOPP_FCONTEXT_ABI can be set to arm/arm64/i386/x86_64/combined/mips/ppc32/ppc64/sparc/sparc64
if (NOT LIBCOPP_FCONTEXT_OS_PLATFORM)
	set(LIBCOPP_FCONTEXT_OS_PLATFORM ${CMAKE_SYSTEM_PROCESSOR})
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
# LIBCOPP_FCONTEXT_ABI can be set to elf_gas/macho_gas/pe_armasm/pe_masm/xcoff_gas
if(NOT LIBCOPP_FCONTEXT_BIN_FORMAT)
	set(LIBCOPP_FCONTEXT_BIN_FORMAT "elf_gas")
	
	if(WIN32 OR CYGWIN)
		if ("${LIBCOPP_FCONTEXT_OS_PLATFORM}" STREQUAL "arm" OR "${LIBCOPP_FCONTEXT_OS_PLATFORM}" STREQUAL "arm64")
			set(LIBCOPP_FCONTEXT_BIN_FORMAT "pe_armasm")
		else()
			set(LIBCOPP_FCONTEXT_BIN_FORMAT "pe_masm")
		endif()
	elseif(APPLE)
		set(LIBCOPP_FCONTEXT_BIN_FORMAT "macho_gas")
	elseif(AIX) # cmake not supported now
		set(LIBCOPP_FCONTEXT_BIN_FORMAT "xcoff_gas")
	endif()
endif()

# ========== set as tool ==========
if(NOT LIBCOPP_FCONTEXT_AS_TOOL)
	set(LIBCOPP_FCONTEXT_AS_TOOL "as")
	
	if(WIN32 OR CYGWIN)
		if ("${LIBCOPP_FCONTEXT_OS_PLATFORM}" STREQUAL "arm" OR "${LIBCOPP_FCONTEXT_OS_PLATFORM}" STREQUAL "arm64")
			set(LIBCOPP_FCONTEXT_AS_TOOL "armasm")
		elseif( "${PLATFORM_SUFFIX}" STREQUAL "64" )
			set(LIBCOPP_FCONTEXT_AS_TOOL "ml64")
		else()
			set(LIBCOPP_FCONTEXT_AS_TOOL "ml")
		endif()
	endif()
endif()