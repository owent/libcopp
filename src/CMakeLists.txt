

EchoWithColor(COLOR GREEN "-- Configure libcopp")

# ========== build library ==========
source_group_by_dir(COPP_SRC_LIST)

add_library(${PROJECT_LIBCOPP_LIB_LINK} ${COPP_SRC_LIST})

install(TARGETS ${PROJECT_LIBCOPP_LIB_LINK}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib${PLATFORM_SUFFIX}
    ARCHIVE DESTINATION lib${PLATFORM_SUFFIX}
)

