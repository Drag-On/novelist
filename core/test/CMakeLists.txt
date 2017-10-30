
project(novelist_core_test)

enable_testing()

add_executable(novelist_core_test
        main.cpp main.h
        macros.h
        datastructures/TreeTest.cpp
        document/SceneDocumentTest.cpp
        util/IdentityTest.cpp
        model/ProjectModelTest.cpp
        view/ProjectViewTest.cpp
        widgets/TextEditorTest.cpp
        datastructures/SortedVectorTest.cpp)

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

enable_cxx17(novelist_core_test)