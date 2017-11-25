/**********************************************************
 * @file   StatsPlugin.cpp
 * @author Jan Möller
 * @date   19.11.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#ifndef NOVELIST_PLUGIN_H
#define NOVELIST_PLUGIN_H

#include <memory>
#include <plugin/BasePlugin.h>
#include "ProjectStatCollector.h"

namespace novelist
{
    class StatsPlugin : public BasePlugin
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "novelist.StatsPlugin" FILE "StatsPlugin.json")

    public:
        bool load(gsl::not_null<Settings*> settings) override;

        void setup(QVector<PluginInfo> const& pluginInfo) override;

        void unload() override;

    private:
        std::unique_ptr<ProjectStatCollector> m_statCollector;
        std::unique_ptr<QAction> m_openStatsAction;
    };
}

#endif //NOVELIST_PLUGIN_H
