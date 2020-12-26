echowithcolor(COLOR GREEN "-- Configure libraries")

add_subdirectory("${PROJECT_LIBCOPP_ROOT_SRC_DIR}/libcopp")

# ========= libcotask =========
if(LIBCOTASK_MACRO_ENABLED)
  add_subdirectory("${PROJECT_LIBCOPP_ROOT_SRC_DIR}/libcotask")
endif()
