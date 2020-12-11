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

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.12")
    find_package(Python3 COMPONENTS Interpreter)
    # Patch for python3 binary
    if (NOT Python3_Interpreter_FOUND AND UNIX)
        find_program(Python3_EXECUTABLE NAMES python3)
        if (Python3_EXECUTABLE)
            get_filename_component(Python3_BIN_DIR ${Python3_EXECUTABLE} DIRECTORY)
            get_filename_component(Python3_ROOT_DIR ${Python3_BIN_DIR} DIRECTORY CACHE)
            find_package(Python3 COMPONENTS Interpreter)
        endif ()
    endif()
    if (Python3_Interpreter_FOUND)
        if (NOT Python_EXECUTABLE)
            get_target_property(Python_EXECUTABLE Python3::Interpreter IMPORTED_LOCATION)
        endif ()
    else ()
        find_package(Python COMPONENTS Interpreter)
        if (TARGET Python::Interpreter)
            if (NOT Python_EXECUTABLE)
                get_target_property(Python_EXECUTABLE Python::Interpreter IMPORTED_LOCATION)
            endif ()
        endif ()
    endif ()
else()
    find_package(PythonInterp)
    if (PYTHONINTERP_FOUND AND NOT Python_EXECUTABLE)
        set(Python_EXECUTABLE ${PYTHON_EXECUTABLE})
    endif()
endif()

if (NOT Python_EXECUTABLE)
    message(WARNING "EchoWithColor will ignore COLOR without python or python3.")
endif ()

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
	
    if (ECHO_WITH_COLOR_COLOR AND Python_EXECUTABLE AND ECHO_WITH_COLOR_TOOL_PATH)
        execute_process(COMMAND ${Python_EXECUTABLE} ${ECHO_WITH_COLOR_TOOL_PATH} -e -c ${ECHO_WITH_COLOR_COLOR} "{0}\r\n" "${ECHO_WITH_COLOR_MSG}")
    else()
        message(${ECHO_WITH_COLOR_MSG})
    endif()
endfunction(EchoWithColor)


