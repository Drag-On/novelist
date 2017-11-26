
if (catch_FOUND)

    project(novelist_stats_test)

    enable_testing()

    add_executable(novelist_stats_test
            main.cpp TextAnalyzerTest.cpp)

    target_include_directories(novelist_stats_test
            PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}
            )

    target_link_libraries(novelist_stats_test
            PRIVATE
            novelist_core
            novelist_stats
            Catch
            )

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        target_compile_options(novelist_stats_test
                PRIVATE
                -Wall -Wextra -Wpedantic
                )
    endif()

    enable_cxx17(novelist_stats_test)

endif(catch_FOUND)