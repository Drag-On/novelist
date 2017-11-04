
project(novelist_languagetool)

add_definitions(-DQT_PLUGIN)

add_library(novelist_languagetool
        SHARED
            src/novelist/LanguageToolPlugin.cpp include/novelist/LanguageToolPlugin.h
            src/novelist/LanguageToolInspector.cpp include/novelist/LanguageToolInspector.h
            src/novelist/SettingsPage_LanguageTool.cpp include/novelist/SettingsPage_LanguageTool.h)

target_include_directories(novelist_languagetool
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include/novelist/
        )

target_link_libraries(novelist_languagetool
        PUBLIC
            novelist_core
            Qt5::Network
        )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(novelist_languagetool
            PRIVATE
                -Wall -Wextra -Wpedantic
            )
endif()

enable_cxx17(novelist_languagetool)