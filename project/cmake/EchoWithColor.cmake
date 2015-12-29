# cmake color echo function
# the final output color associated with terminal settings
# using [print_color.py](https://github.com/owent-utils/python/blob/master/print_color.py) for cross platform color rending. 
#   so [python](https://www.python.org/) and [print_color.py](https://github.com/owent-utils/python/blob/master/print_color.py) is required.
#   python2 or python3 are all supported.
# usage: EchoWithColor( [
#	[COLOR RED|GREEN|YELLOW|BLUE|MAGENTA|CYAN] 
#	[message1 [message2 ...]]] ...  
# )
#

find_package(PythonInterp)
if (NOT PYTHONINTERP_FOUND)
	message(WARNING "EchoWithColor will ignore COLOR without python.")
endif()

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/print_color.py")
    set(ECHO_WITH_COLOR_TOOL_PATH "${CMAKE_CURRENT_LIST_DIR}/print_color.py")
else()
    message(WARNING "EchoWithColor will ignore COLOR without print_color.py.")
endif()

function(EchoWithColor)
	# ${ARGV}, ${ARGN}
	
	set(ECHO_WITH_COLOR_COLOR "")
	set(ECHO_WITH_COLOR_FLAG "false")
    set(ECHO_WITH_COLOR_MSG "")
	
	foreach (msg IN LISTS ARGV)
        if ( "${msg}" STREQUAL "COLOR" )
			set(ECHO_WITH_COLOR_FLAG "true")
		elseif( "${ECHO_WITH_COLOR_FLAG}" STREQUAL "true" )
			set(ECHO_WITH_COLOR_FLAG "false")
            set(ECHO_WITH_COLOR_COLOR ${msg})
		else()
            set(ECHO_WITH_COLOR_MSG "${ECHO_WITH_COLOR_MSG}${msg}")
		endif()
	endforeach()
	
    if (ECHO_WITH_COLOR_COLOR AND PYTHONINTERP_FOUND AND ECHO_WITH_COLOR_TOOL_PATH)
        execute_process(COMMAND ${PYTHON_EXECUTABLE} ${ECHO_WITH_COLOR_TOOL_PATH} -e -c ${ECHO_WITH_COLOR_COLOR} "{0}\r\n" "${ECHO_WITH_COLOR_MSG}")
    else()
        message(${ECHO_WITH_COLOR_MSG})
    endif()
endfunction(EchoWithColor)


