/**********************************************************
 * @file   MainPlugin.cpp
 * @author Jan Möller
 * @date   11.03.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include "MainPlugin.h"

namespace novelist
{
    bool MainPlugin::load()
    {
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
