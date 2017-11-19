/**********************************************************
 * @file   MainPlugin.cpp
 * @author Jan Möller
 * @date   11.03.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#ifndef NOVELIST_PLUGIN_H
#define NOVELIST_PLUGIN_H

#include <QtCore/QObject>
#include <plugin/Plugin.h>

namespace novelist
{
    class StatsPlugin : public QObject, public Plugin
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "novelist.StatsPlugin" FILE "StatsPlugin.json")
        Q_INTERFACES(novelist::Plugin)

    public:
        bool load(gsl::not_null<Settings*> settings) override;

        void setup(QVector<PluginInfo> const& pluginInfo) override;

        void unload() override;

    private:
    };
}

#endif //NOVELIST_PLUGIN_H
