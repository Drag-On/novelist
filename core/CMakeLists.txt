
project(novelist_core)

add_library(novelist_core
        SHARED
        resources/resources.qrc
        src/novelist/lang/Language.cpp include/novelist/lang/Language.h
        include/novelist/plugin/IPlugin.h
        include/novelist/plugin/PluginInfo.h
        src/novelist/plugin/PluginManager.cpp include/novelist/plugin/PluginManager.h
        src/novelist/model/ProjectModel.cpp include/novelist/model/ProjectModel.h
        src/novelist/model/ModelPath.cpp include/novelist/model/ModelPath.h
        include/novelist/datastructures/Tree.h
        include/novelist/functional/Overloaded.h
        include/novelist/identity/Identity.h
        src/novelist/view/ProjectView.cpp include/novelist/view/ProjectView.h
        src/novelist/widgets/LanguagePicker.cpp include/novelist/widgets/LanguagePicker.h
        src/novelist/widgets/texteditor/TextEditor.cpp include/novelist/widgets/texteditor/TextEditor.h
        src/novelist/widgets/texteditor/TextAnnotation.cpp include/novelist/widgets/texteditor/TextAnnotation.h
        src/novelist/widgets/SceneTabWidget.cpp include/novelist/widgets/SceneTabWidget.h
        src/novelist/document/SceneDocument.cpp include/novelist/document/SceneDocument.h
        src/novelist/document/TextMarker.cpp include/novelist/document/TextMarker.h
        src/novelist/windows/ProjectPropertiesWindow.cpp include/novelist/windows/ProjectPropertiesWindow.h
        src/novelist/windows/NoteEditWindow.cpp include/novelist/windows/NoteEditWindow.h
        src/novelist/windows/MainWindow.cpp include/novelist/windows/MainWindow.h
        src/novelist/util/ConnectionWrapper.cpp include/novelist/util/ConnectionWrapper.h
        src/novelist/util/Connection.cpp include/novelist/util/Connection.h
        src/novelist/util/DelegateAction.cpp include/novelist/util/DelegateAction.h
        include/novelist/inspection/IInsight.h
        src/novelist/inspection/InsightModel.cpp include/novelist/inspection/InsightModel.h
        )

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