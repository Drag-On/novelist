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
    void InspectionPlugin::setup(QVector<PluginInfo> const& pluginInfo)
    {
        BasePlugin::setup(pluginInfo);

        auto* window = mainWindow();
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
        qWarning() << "Unable to register inspection plugin.";
    }

    bool InspectionPlugin::load(gsl::not_null<Settings*>)
    {
        return true;
    }

    void InspectionPlugin::unload()
    {
    }
}