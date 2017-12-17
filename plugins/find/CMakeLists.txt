
project(novelist_find)

add_definitions(-DQT_PLUGIN)

add_library(novelist_find
        SHARED
        include/novelist/FindPlugin.h src/novelist/FindPlugin.cpp
        src/novelist/FindWidget.cpp include/novelist/FindWidget.h)

target_include_directories(novelist_find
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/novelist/
        )

target_link_libraries(novelist_find
        PUBLIC
        novelist_core
        )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(novelist_find
            PRIVATE
            -Wall -Wextra -Wpedantic
            )
endif ()

enable_cxx17(novelist_find)
enable_i18n(novelist_find)
