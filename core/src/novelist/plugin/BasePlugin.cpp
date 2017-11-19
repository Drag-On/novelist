/**********************************************************
 * @file   BasePlugin.cpp
 * @author jan
 * @date   11/19/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include "plugin/BasePlugin.h"
#include <QDebug>

namespace novelist {
    void BasePlugin::setup(QVector<PluginInfo> const& /*pluginInfo*/)
    {
        for(QWidget* w : qApp->topLevelWidgets()) {
            auto* window = dynamic_cast<MainWindow*>(w);
            if (window) {
                m_mainWnd = window;
                return;
            }

        }
        qWarning() << "Unable to find MainWindow.";
    }

    MainWindow* BasePlugin::mainWindow() noexcept
    {
        return m_mainWnd;
    }

    MainWindow const* BasePlugin::mainWindow() const noexcept
    {
        return m_mainWnd;
    }
}