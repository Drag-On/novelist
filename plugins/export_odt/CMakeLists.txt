
project(novelist_export_odt)

add_definitions(-DQT_PLUGIN)

add_library(novelist_export_odt
        SHARED
        include/novelist/ExportOdtPlugin.h src/novelist/ExportOdtPlugin.cpp
        )

target_include_directories(novelist_export_odt
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/novelist/
        )

target_link_libraries(novelist_export_odt
        PUBLIC
        novelist_core
        zip
        )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(novelist_export_odt
            PRIVATE
            -Wall -Wextra -Wpedantic
            )
endif ()

set_property(TARGET novelist_export_odt PROPERTY POSITION_INDEPENDENT_CODE ON)

enable_cxx17(novelist_export_odt)
