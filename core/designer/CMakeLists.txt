project(novelist_core_designer)

add_library(novelist_core_designer
        SHARED
            main.h main.cpp
            LanguagePickerPlugin.h LanguagePickerPlugin.cpp
            ProjectViewPlugin.cpp ProjectViewPlugin.h)

target_link_libraries(novelist_core_designer
        PUBLIC
            novelist_core
            Qt5::UiPlugin
        )

set_target_properties(novelist_core_designer
        PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED YES
            CXX_EXTENSIONS NO
        )