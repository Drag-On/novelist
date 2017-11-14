
project(novelist_core)

add_library(novelist_core
        SHARED
        resources/resources.qrc
        resources/icons/tango/tango.qrc
        include/novelist/plugin/Plugin.h
        include/novelist/plugin/PluginInfo.h
        src/novelist/plugin/InspectionPlugin.cpp include/novelist/plugin/InspectionPlugin.h
        src/novelist/plugin/PluginManager.cpp include/novelist/plugin/PluginManager.h
        src/novelist/model/ProjectModel.cpp include/novelist/model/ProjectModel.h
        src/novelist/model/ModelPath.cpp include/novelist/model/ModelPath.h
        src/novelist/model/Language.cpp include/novelist/model/Language.h
        include/novelist/datastructures/Tree.h
        include/novelist/datastructures/SortedVector.h
        src/novelist/view/ProjectView.cpp include/novelist/view/ProjectView.h
        src/novelist/view/InsightView.cpp include/novelist/view/InsightView.h
        src/novelist/widgets/LanguagePicker.cpp include/novelist/widgets/LanguagePicker.h
        src/novelist/widgets/texteditor/TextEditor.cpp include/novelist/widgets/texteditor/TextEditor.h
        src/novelist/widgets/texteditor/TextEditorInsightManager.cpp include/novelist/widgets/texteditor/TextEditorInsightManager.h
        src/novelist/widgets/texteditor/InsightModel.cpp include/novelist/widgets/texteditor/InsightModel.h
        src/novelist/widgets/SceneTabWidget.cpp include/novelist/widgets/SceneTabWidget.h
        include/novelist/widgets/texteditor/Inspector.h
        src/novelist/document/SceneDocument.cpp include/novelist/document/SceneDocument.h
        src/novelist/document/SceneDocumentInsightManager.cpp include/novelist/document/SceneDocumentInsightManager.h
        src/novelist/document/Insight.cpp include/novelist/document/Insight.h
        src/novelist/document/BaseInsight.cpp include/novelist/document/BaseInsight.h
        src/novelist/document/NoteInsight.cpp include/novelist/document/NoteInsight.h
        src/novelist/document/AutoInsight.cpp include/novelist/document/AutoInsight.h
        src/novelist/document/GrammarInsight.cpp include/novelist/document/GrammarInsight.h
        src/novelist/document/SpellingInsight.cpp include/novelist/document/SpellingInsight.h
        src/novelist/document/TypographyInsight.cpp include/novelist/document/TypographyInsight.h
        include/novelist/document/InsightFactory.h
        src/novelist/windows/ProjectPropertiesWindow.cpp include/novelist/windows/ProjectPropertiesWindow.h
        src/novelist/windows/NoteEditWindow.cpp include/novelist/windows/NoteEditWindow.h
        src/novelist/windows/MainWindow.cpp include/novelist/windows/MainWindow.h
        src/novelist/windows/SettingsWindow.cpp include/novelist/windows/SettingsWindow.h
        include/novelist/util/Overloaded.h
        include/novelist/util/Identity.h
        src/novelist/util/ConnectionWrapper.cpp include/novelist/util/ConnectionWrapper.h
        src/novelist/util/Connection.cpp include/novelist/util/Connection.h
        src/novelist/util/DelegateAction.cpp include/novelist/util/DelegateAction.h
        src/novelist/util/MenuHelper.cpp include/novelist/util/MenuHelper.h
        src/novelist/util/TranslationManager.cpp include/novelist/util/TranslationManager.h
        include/novelist/settings/SettingsPage.h
        src/novelist/settings/Settings.cpp include/novelist/settings/Settings.h
        src/novelist/settings/SettingsPage_General.cpp include/novelist/settings/SettingsPage_General.h
        src/novelist/settings/SettingsPage_Editor.cpp include/novelist/settings/SettingsPage_Editor.h
        src/novelist/widgets/FilePicker.cpp include/novelist/widgets/FilePicker.h
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
            Qt5::Concurrent
            GSL
        )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(novelist_core
            PRIVATE
                -Wall -Wextra -Wpedantic
            )
endif ()

# Output library to binary dir instead of subfolder because windows can't link to libraries not in standard locations.
set_target_properties(novelist_core
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        )

enable_cxx17(novelist_core)
enable_i18n(novelist_core)

add_subdirectory(test)
add_subdirectory(designer)
