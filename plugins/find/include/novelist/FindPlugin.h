/**********************************************************
 * @file   FindPlugin.cpp
 * @author Jan Möller
 * @date   17.12.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#ifndef NOVELIST_FINDPLUGIN_H
#define NOVELIST_FINDPLUGIN_H

#include <memory>
#include <plugin/BasePlugin.h>

namespace novelist
{
    class FindWidget;

    class FindPlugin : public BasePlugin
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "novelist.FindPlugin" FILE "FindPlugin.json")

    public:
        bool load(gsl::not_null<Settings*> settings) override;

        void setup(QVector<PluginInfo> const& pluginInfo) override;

        void unload() override;

    private:
        QDockWidget* m_dockWidget = nullptr;
        FindWidget* m_findWidget = nullptr;
    };
}

#endif //NOVELIST_FINDPLUGIN_H
