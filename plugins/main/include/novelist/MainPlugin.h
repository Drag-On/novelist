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

#include <memory>
#include <QtCore/QObject>
#include <windows/MainWindow.h>
#include <plugin/Plugin.h>

namespace novelist
{
    class MainPlugin : public QObject, public Plugin
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "novelist.MainPlugin" FILE "MainPlugin.json")
        Q_INTERFACES(novelist::Plugin)

    public:
        bool load(gsl::not_null<Settings*> settings) override;

        void setup(QVector<PluginInfo> const& pluginInfo) override;

        void unload() override;

    private:
        std::unique_ptr<MainWindow> m_mainWindow;
    };
}

#endif //NOVELIST_PLUGIN_H
