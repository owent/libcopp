#cmake color echo function
#the final output color associated with terminal settings
#usage: EchoWithColor( [
#	[COLOR RED|GREEN|YELLOW|BLUE|PURPLE|CYAN] 
#	[message1 [message2 ...]]] ...  
#)

if (WIN32)
	message(WARNING "EchoWithColor will ignore COLOR in Win32 and Msys.")
endif()

function(EchoWithColor)
	# ${ARGV}, ${ARGN}
	
	set(ECHO_WITH_COLOR_CMD "echo")
	set(ECHO_WITH_COLOR_CMD_DP "")
    if(UNIX OR CYGWIN OR APPLE)
        set(TAG_RED     "\\033[31;1m")
        set(TAG_GREEN   "\\033[32;1m")
        set(TAG_YELLOW  "\\033[33;1m")
        set(TAG_BLUE    "\\033[34;1m")
        set(TAG_PURPLE  "\\033[35;1m")
        set(TAG_CYAN    "\\033[36;1m")
        set(TAG_RESET   "\\033[;0m")
		
		set(ECHO_WITH_COLOR_CMD_DP "-e")
		
	elseif(WIN32)
		set(TAG_RED     "")
        set(TAG_GREEN   "")
        set(TAG_YELLOW  "")
        set(TAG_BLUE    "")
        set(TAG_PURPLE  "")
        set(TAG_CYAN    "")
        set(TAG_RESET   "")
	endif()
	
	set(ECHO_WITH_COLOR_PREFIX "")
	set(ECHO_WITH_COLOR_SUFFIX "")
	set(ECHO_WITH_COLOR_FLAG "false")
	
	foreach (msg IN LISTS ARGV)
        if ( "${msg}" STREQUAL "COLOR" )
			set(ECHO_WITH_COLOR_FLAG "true")
		elseif( "${ECHO_WITH_COLOR_FLAG}" STREQUAL "true" )
			set(ECHO_WITH_COLOR_FLAG "false")
			if ("${msg}" STREQUAL "RED")
				set(ECHO_WITH_COLOR_PREFIX "${TAG_RED}")
				set(ECHO_WITH_COLOR_SUFFIX "${TAG_RESET}")
			elseif ("${msg}" STREQUAL "GREEN")
				set(ECHO_WITH_COLOR_PREFIX "${TAG_GREEN}")
				set(ECHO_WITH_COLOR_SUFFIX "${TAG_RESET}")
			elseif ("${msg}" STREQUAL "YELLOW")
				set(ECHO_WITH_COLOR_PREFIX "${TAG_YELLOW}")
				set(ECHO_WITH_COLOR_SUFFIX "${TAG_RESET}")
			elseif ("${msg}" STREQUAL "BLUE")
				set(ECHO_WITH_COLOR_PREFIX "${TAG_BLUE}")
				set(ECHO_WITH_COLOR_SUFFIX "${TAG_RESET}")
			elseif ("${msg}" STREQUAL "PURPLE")
				set(ECHO_WITH_COLOR_PREFIX "${TAG_PURPLE}")
				set(ECHO_WITH_COLOR_SUFFIX "${TAG_RESET}")
			elseif ("${msg}" STREQUAL "CYAN")
				set(ECHO_WITH_COLOR_PREFIX "${TAG_CYAN}")
				set(ECHO_WITH_COLOR_SUFFIX "${TAG_RESET}")
			else ()
				message(WARNING "EchoWithColor ${msg} not supported.")
			endif()
		else()
			execute_process(COMMAND ${ECHO_WITH_COLOR_CMD} ${ECHO_WITH_COLOR_CMD_DP} "${ECHO_WITH_COLOR_PREFIX}${msg}${ECHO_WITH_COLOR_SUFFIX}")
		endif()
	endforeach()
	
endfunction(EchoWithColor)


