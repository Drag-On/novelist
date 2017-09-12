
project(novelist_launcher)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(novelist_launcher
            src/novelist/main.cpp
        )

target_include_directories(novelist_launcher
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include/novelist/
        )

target_link_libraries(novelist_launcher
        PUBLIC
            novelist_core
        )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(novelist_launcher
            PRIVATE
                -Wall -Wextra -Wpedantic
            )
endif()

enable_cxx17(novelist_launcher)