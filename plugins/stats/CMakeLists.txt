
project(novelist_stats)

add_definitions(-DQT_PLUGIN)

add_library(novelist_stats
        SHARED
        include/novelist/StatsPlugin.h src/novelist/StatsPlugin.cpp
        src/novelist/SettingsPage_Stats.cpp include/novelist/SettingsPage_Stats.h
        src/novelist/ProjectStatCollector.cpp include/novelist/ProjectStatCollector.h
        )

target_include_directories(novelist_stats
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/novelist/
        )

target_link_libraries(novelist_stats
        PUBLIC
        novelist_core
        )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(novelist_stats
            PRIVATE
            -Wall -Wextra -Wpedantic
            )
endif ()

enable_cxx17(novelist_stats)