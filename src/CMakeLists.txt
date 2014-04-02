

EchoWithColor(COLOR GREEN "-- Configure libraries")

add_subdirectory("${PROJECT_ROOT_SRC_DIR}/libcopp")

# ========= libcotask =========  
if (LIBCOTASK_ENABLE)
    add_subdirectory("${PROJECT_ROOT_SRC_DIR}/libcotask")
endif()
