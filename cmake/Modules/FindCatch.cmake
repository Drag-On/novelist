find_path(
        CATCH_INCLUDE_DIR
        NAMES catch.hpp
        HINTS ${CATCH_DIR}/single_include/
        DOC "catch include directory"
)


set(CATCH_INCLUDE_DIRS ${CATCH_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(catch DEFAULT_MSG
        CATCH_INCLUDE_DIR)
mark_as_advanced (CATCH_INCLUDE_DIR)

if (catch_FOUND)
    add_library(Catch INTERFACE)
    target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})
endif ()