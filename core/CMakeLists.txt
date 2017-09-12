
project(novelist_core)

add_library(novelist_core
        SHARED
        resources/resources.qrc
        src/novelist/lang/Language.cpp include/novelist/lang/Language.h
        include/novelist/plugin/IPlugin.h
        include/novelist/plugin/PluginInfo.h
        src/novelist/plugin/PluginManager.cpp include/novelist/plugin/PluginManager.h
        src/novelist/model/ProjectModel.cpp include/novelist/model/ProjectModel.h
        include/novelist/datastructures/Tree.h
        include/novelist/functional/Overloaded.h
        include/novelist/identity/Identity.h
        src/novelist/view/ProjectView.cpp include/novelist/view/ProjectView.h
        src/novelist/widgets/LanguagePicker.cpp include/novelist/widgets/LanguagePicker.h
        src/novelist/windows/ProjectPropertiesWindow.cpp include/novelist/windows/ProjectPropertiesWindow.h
        src/novelist/widgets/TextEditor.cpp include/novelist/widgets/TextEditor.h
        src/novelist/widgets/SceneTabWidget.cpp include/novelist/widgets/SceneTabWidget.h
        src/novelist/datastructures/SceneDocument.cpp include/novelist/datastructures/SceneDocument.h
        src/novelist/windows/MainWindow.cpp include/novelist/windows/MainWindow.h)

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

enable_cxx17(novelist_core)

add_subdirectory(test)
add_subdirectory(designer)