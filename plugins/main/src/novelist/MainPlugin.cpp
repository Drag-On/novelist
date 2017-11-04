/**********************************************************
 * @file   MainPlugin.cpp
 * @author Jan Möller
 * @date   11.03.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <settings/SettingsPage_General.h>
#include <settings/SettingsPage_Editor.h>
#include "MainPlugin.h"

namespace novelist
{
    bool MainPlugin::load(gsl::not_null<Settings*> settings)
    {
        settings->registerPage(std::make_unique<SettingsPage_General_Creator>());
        settings->registerPage(std::make_unique<SettingsPage_Editor_Creator>());

        m_mainWindow = std::make_unique<MainWindow>();
        m_mainWindow->show();

        return true;
    }

    void MainPlugin::setup(QVector<PluginInfo> const& /* pluginInfo */)
    {
    }

    void MainPlugin::unload()
    {
        m_mainWindow.reset();
    }
}
