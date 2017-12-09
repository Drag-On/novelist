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

    bool BasePlugin::load(gsl::not_null<Settings*> )
    {
        for(QWidget* w : qApp->topLevelWidgets()) {
            auto* window = dynamic_cast<MainWindow*>(w);
            if (window) {
                m_mainWnd = window;
                return true;
            }

        }
        qWarning() << "Unable to find MainWindow.";
        return false;
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