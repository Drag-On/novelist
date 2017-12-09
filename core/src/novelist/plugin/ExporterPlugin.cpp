/**********************************************************
 * @file   ExporterPlugin.cpp
 * @author jan
 * @date   12/3/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "plugin/ExporterPlugin.h"
#include <QDebug>

namespace novelist {
    bool ExporterPlugin::load(gsl::not_null<Settings*> settings)
    {
        if (!BasePlugin::load(settings))
            return false;

        auto exportMenu = mainWindow()->findChild<QMenu*>("menuExport");
        if (exportMenu == nullptr)
            return false;

        exportMenu->addAction(exportAction());

        return true;
    }

    void ExporterPlugin::setup(QVector<PluginInfo> const& /*pluginInfo*/)
    {
    }

    void ExporterPlugin::unload()
    {
    }

    gsl::not_null<ProjectModel const*> ExporterPlugin::curProject() const noexcept
    {
        if (mainWindow()->project())
            return mainWindow()->project();
        else {
            qCritical() << "Current project is null. Cannot export a null project.";
            std::terminate();
        }
    }
}