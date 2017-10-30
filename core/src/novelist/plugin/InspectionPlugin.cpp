/**********************************************************
 * @file   InspectionPlugin.cpp
 * @author jan
 * @date   10/30/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <windows/MainWindow.h>
#include <widgets/SceneTabWidget.h>
#include "plugin/InspectionPlugin.h"

namespace novelist {
    void InspectionPlugin::setup(QVector<PluginInfo> const& /*pluginInfo*/)
    {
        for(QWidget* w : qApp->topLevelWidgets()) {
            auto* window = dynamic_cast<MainWindow*>(w);
            if (window) {
                auto* tabWidget = window->findChild<SceneTabWidget*>();
                if (tabWidget) {
                    auto inspector = createInspector();
                    if (inspector) {
                        tabWidget->registerInspector(std::move(inspector));
                        return;
                    }
                }
            }
        }
        qWarning() << "Unable to register inspection plugin.";
    }

    bool InspectionPlugin::load()
    {
        return true;
    }

    void InspectionPlugin::unload()
    {
    }
}