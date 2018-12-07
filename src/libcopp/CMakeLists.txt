

EchoWithColor(COLOR GREEN "-- Configure libcopp")


# ========== build library ==========
source_group_by_dir(COPP_SRC_LIST)

if (LIBCOPP_FCONTEXT_USE_TSX)
    add_compiler_define(LIBCOPP_FCONTEXT_USE_TSX=1)
endif()

add_library(${PROJECT_LIBCOPP_LIB_LINK} ${COPP_SRC_LIST})

install(TARGETS ${PROJECT_LIBCOPP_LIB_LINK}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib${PLATFORM_SUFFIX}
    ARCHIVE DESTINATION lib${PLATFORM_SUFFIX}
)

install(DIRECTORY "${PROJECT_LIBCOPP_INC_DIR}"
    DESTINATION include
    USE_SOURCE_PERMISSIONS
    PATTERN ".svn" EXCLUDE
    PATTERN ".git" EXCLUDE
    PATTERN "*.in" EXCLUDE
)
