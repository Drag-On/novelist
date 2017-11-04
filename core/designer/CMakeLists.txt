project(novelist_core_designer)

if (NOT NoDesigner)

    add_library(novelist_core_designer
            SHARED
            main.h main.cpp
            LanguagePickerPlugin.h LanguagePickerPlugin.cpp
            ProjectViewPlugin.cpp ProjectViewPlugin.h
            SceneTabWidgetPlugin.cpp SceneTabWidgetPlugin.h
            InsightViewPlugin.cpp InsightViewPlugin.h
            FilePickerPlugin.cpp FilePickerPlugin.h)

    target_link_libraries(novelist_core_designer
            PUBLIC
            novelist_core
            Qt5::UiPlugin
            )

    enable_cxx17(novelist_core_designer)

endif ()