
project(novelist_core)

add_library(novelist_core
        SHARED
        resources/resources.qrc
        src/novelist/lang/Language.cpp include/novelist/lang/Language.h
        src/novelist/model/ProjectModel.cpp include/novelist/model/ProjectModel.h
        include/novelist/datastructures/Tree.h
        include/novelist/functional/Overloaded.h
        include/novelist/identity/Identity.h
        src/novelist/view/ProjectView.cpp include/novelist/view/ProjectView.h
        src/novelist/widgets/LanguagePicker.cpp include/novelist/widgets/LanguagePicker.h)

target_include_directories(novelist_core
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include/novelist/
        )

target_link_libraries(novelist_core
        PUBLIC
            Qt5::Core
            Qt5::Widgets
            Qt5::Gui
            GSL
        )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(novelist_core
            PRIVATE
                -Wall -Wextra -Wpedantic
            )
endif()

set_target_properties(novelist_core
        PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED YES
            CXX_EXTENSIONS NO
        )

add_subdirectory(test)