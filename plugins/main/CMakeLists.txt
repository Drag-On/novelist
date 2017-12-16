
project(novelist_main)

add_definitions(-DQT_PLUGIN)

add_library(novelist_main
        SHARED
            include/novelist/MainPlugin.h src/novelist/MainPlugin.cpp
        )

target_include_directories(novelist_main
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include/novelist/
        )

target_link_libraries(novelist_main
        PUBLIC
            novelist_core
        )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(novelist_main
            PRIVATE
                -Wall -Wextra -Wpedantic
            )
endif()

enable_cxx17(novelist_main)
enable_i18n(novelist_main)
