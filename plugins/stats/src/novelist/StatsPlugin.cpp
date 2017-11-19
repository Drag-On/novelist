/**********************************************************
 * @file   StatsPlugin.cpp
 * @author Jan Möller
 * @date   19.11.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include "StatsPlugin.h"
#include <SettingsPage_Stats.h>

namespace novelist
{
    bool StatsPlugin::load(gsl::not_null<Settings*> settings)
    {
        m_statCollector = std::make_unique<ProjectStatCollector>();

        settings->registerPage(std::make_unique<SettingsPage_Stats_Creator>());

        return true;
    }

    void StatsPlugin::setup(QVector<PluginInfo> const&  pluginInfo )
    {
        BasePlugin::setup(pluginInfo);

        connect(mainWindow(), &MainWindow::projectAboutToChange, m_statCollector.get(), &ProjectStatCollector::onProjectAboutToChange);
        connect(mainWindow(), &MainWindow::projectChanged, m_statCollector.get(), &ProjectStatCollector::onProjectChanged);
    }

    void StatsPlugin::unload()
    {
        m_statCollector.reset();
    }
}
