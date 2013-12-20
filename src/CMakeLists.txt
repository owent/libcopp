

EchoWithColor(COLOR GREEN "-- Configure libcopp")

# ========== build library ==========
add_library(${PROJECT_LIBCOPP_LIB_LINK} ${COPP_SRC_LIST})
install(TARGETS ${PROJECT_LIBCOPP_LIB_LINK}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib${PLATFORM_SUFFIX}
    ARCHIVE DESTINATION lib${PLATFORM_SUFFIX}
)

