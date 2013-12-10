# cmake 递归包含头文件目录模块 
# include_directory_recurse [dir1 [dir2 [...]]] 
macro(include_directory_recurse)
	foreach(basedir ${ARGV})
	    file(GLOB_RECURSE SRC_HEADER_LIST_H "${basedir}/*.h" "${basedir}/*.hpp")
		set(LAST_HEAD_FILE_DIR "  ")
		foreach(src ${SRC_HEADER_LIST_H})
			#去掉文件名，截取路径
			string(REGEX REPLACE "(.+)[/\\].+\\.h(pp)?$" "\\1" CUR_HEAD_FILE_DIR ${src})
			
			if ( NOT "${CUR_HEAD_FILE_DIR}" STREQUAL "${LAST_HEAD_FILE_DIR}" )
				include_directories(${CUR_HEAD_FILE_DIR})
				set(LAST_HEAD_FILE_DIR "${CUR_HEAD_FILE_DIR}")
				
				message(STATUS "Recurse Include -- ${LAST_HEAD_FILE_DIR}")
			endif()
			
		endforeach()
	endforeach()
endmacro(include_directory_recurse)

# cmake 递归包含macro声明模块 
# include_macro_recurse [FILTER filter] | [dir1 [dir2 [...]]]
macro(include_macro_recurse)
	set(INCLUDE_MACRO_RECURSE_FILTER "*.macro.cmake")
	set(INCLUDE_MACRO_RECURSE_FILTER_FLAG "false")

	foreach(basedir ${ARGV})
        if ( "${basedir}" STREQUAL "FILTER" )
			set(INCLUDE_MACRO_RECURSE_FILTER_FLAG "true")
		elseif( "${INCLUDE_MACRO_RECURSE_FILTER_FLAG}" STREQUAL "true" )
			set(INCLUDE_MACRO_RECURSE_FILTER_FLAG "false")
			set(INCLUDE_MACRO_RECURSE_FILTER "${basedir}")
		else()
			file(GLOB_RECURSE ALL_MACRO_FILES "${basedir}/${INCLUDE_MACRO_RECURSE_FILTER}")
			foreach(macro_file ${ALL_MACRO_FILES})
				message(STATUS "Macro File Found: ${macro_file}")
				include("${macro_file}")
			endforeach()
		endif()
	endforeach()
endmacro(include_macro_recurse)


# cmake 递归添加工程列表模块 
# add_project_recurse [RECURSE] | [dir1 [dir2 [...]]] 
macro(add_project_recurse)
	set(INCLUDE_PROJECT_RECURSE_RECURSE_FLAG "false")
	
	foreach(basedir ${ARGV})
        if( "${basedir}" STREQUAL "RECURSE" )
		    set(INCLUDE_PROJECT_RECURSE_RECURSE_FLAG "true")
		else()
		    if("${INCLUDE_PROJECT_RECURSE_RECURSE_FLAG}" STREQUAL "true")
			    file(GLOB_RECURSE ALL_PROJECT_FILES "${basedir}/*")
			else()
			    file(GLOB ALL_PROJECT_FILES "${basedir}/*")
			endif()
			foreach(project_dir ${ALL_PROJECT_FILES})
			    if(IS_DIRECTORY "${project_dir}" AND EXISTS "${project_dir}/CMakeLists.txt")
				    message(STATUS "Project Directory Found: ${project_dir}")
				    add_subdirectory("${project_dir}")
				endif()
			endforeach()
		endif()
	endforeach()
endmacro(add_project_recurse)

# cmake VC 源文件分组  
# source_group_by_dir [Source List Var1 [Source List Var2 [...]]]
macro(source_group_by_dir)
    if(MSVC)
        foreach(source_files ${ARGV})
            set(sgbd_cur_dir ${CMAKE_CURRENT_SOURCE_DIR})
            foreach(sgbd_file ${${source_files}})
                string(REGEX REPLACE ${sgbd_cur_dir}/\(.*\) \\1 sgbd_fpath ${sgbd_file})
                string(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
                string(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
                string(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
                if(sgbd_nogroup)
                    set(sgbd_group_name "\\")
                endif(sgbd_nogroup)
                source_group(${sgbd_group_name} FILES ${sgbd_file})
            endforeach(sgbd_file)
        endforeach()
    endif(MSVC)
endmacro(source_group_by_dir)

