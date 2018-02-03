
project(novelist_slim)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(novelist_slim
        src/novelist/main.cpp
        )

target_include_directories(novelist_slim
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/novelist/
        )

target_link_libraries(novelist_slim
        PUBLIC
        novelist_core
        )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(novelist_slim
            PRIVATE
            -Wall -Wextra -Wpedantic
            )
endif()

enable_cxx17(novelist_slim)