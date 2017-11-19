/**********************************************************
 * @file   MainPlugin.cpp
 * @author Jan Möller
 * @date   11.03.17
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
        settings->registerPage(std::make_unique<SettingsPage_Stats_Creator>());

        return true;
    }

    void StatsPlugin::setup(QVector<PluginInfo> const& /* pluginInfo */)
    {
    }

    void StatsPlugin::unload()
    {
    }
}
