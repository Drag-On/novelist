
project(novelist_core_test)

enable_testing()

add_executable(novelist_core_test
        main.cpp main.h
        datastructures/TreeTest.cpp
        datastructures/SceneDocumentTest.cpp
        identity/IdentityTest.cpp
        model/ProjectModelTest.cpp
        view/ProjectViewTest.cpp
        widgets/TextEditorTest.cpp
        )

target_include_directories(novelist_core_test
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_link_libraries(novelist_core_test
        PRIVATE
            novelist_core
            Catch
        )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(novelist_core_test
            PRIVATE
                -Wall -Wextra -Wpedantic
            )
endif()

set_target_properties(novelist_core_test
        PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED YES
            CXX_EXTENSIONS NO
        )