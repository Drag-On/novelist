/**********************************************************
 * @file   FindPlugin.cpp
 * @author Jan Möller
 * @date   17.12.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include "FindPlugin.h"
#include <QDockWidget>
#include <util/TranslationManager.h>
#include "FindWidget.h"

namespace novelist {
    bool FindPlugin::load(gsl::not_null<Settings*> settings)
    {
        if (!BasePlugin::load(settings))
            return false;

        auto langDir = QDir(QApplication::applicationDirPath() + "/plugins/find");
        TranslationManager::instance().registerInDirectory(langDir, "novelist_find");

        auto mainWin = mainWindow();
        if (!mainWin) {
            qWarning() << "Find plugin can't access main window.";
            return false;
        }
        m_dockWidget = new QDockWidget(tr("Find / Replace"), mainWin);
        m_findWidget = new FindWidget(m_dockWidget);
        m_dockWidget->setWidget(m_findWidget);
        mainWin->addDockWidget(Qt::BottomDockWidgetArea, m_dockWidget);
        mainWin->tabifyDockWidget(m_dockWidget, mainWin->findChild<QDockWidget*>("dockInsights"));

        return true;
    }

    void FindPlugin::setup(QVector<PluginInfo> const& /*pluginInfo*/)
    {
        connect(mainWindow(), &MainWindow::languageChanged, [this] {
            m_dockWidget->setWindowTitle(tr("Find / Replace"));
        });
    }

    void FindPlugin::unload()
    {
    }
}
