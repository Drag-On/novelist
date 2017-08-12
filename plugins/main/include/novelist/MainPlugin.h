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
#include <plugin/IPlugin.h>

namespace novelist
{
    class MainPlugin : public QObject, public IPlugin
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "novelist.MainPlugin" FILE "MainPlugin.json")
        Q_INTERFACES(novelist::IPlugin)

    public:
        bool load() override;

        void setup(QVector<PluginInfo> const& pluginInfo) override;

        void unload() override;

    private:
        std::unique_ptr<MainWindow> m_mainWindow;
    };
}

#endif //NOVELIST_PLUGIN_H
