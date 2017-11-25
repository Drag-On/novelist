/**********************************************************
 * @file   StatsPlugin.cpp
 * @author Jan Möller
 * @date   19.11.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include "StatsPlugin.h"
#include "StatsDialog.h"
#include <SettingsPage_Stats.h>

namespace novelist {
    bool StatsPlugin::load(gsl::not_null<Settings*> settings)
    {
        m_statCollector = std::make_unique<ProjectStatCollector>();
        m_openStatsAction = std::make_unique<QAction>(tr("Statistics"));
        m_openStatsAction->setEnabled(false);

        settings->registerPage(std::make_unique<SettingsPage_Stats_Creator>());

        return true;
    }

    void StatsPlugin::setup(QVector<PluginInfo> const& pluginInfo)
    {
        BasePlugin::setup(pluginInfo);

        auto inspectionMenu = mainWindow()->findChild<QMenu*>("menu_Inspection");
        if (inspectionMenu) {
            inspectionMenu->addSeparator();
            inspectionMenu->addAction(m_openStatsAction.get());
        }

        connect(mainWindow(), &MainWindow::projectAboutToChange, m_statCollector.get(),
                &ProjectStatCollector::onProjectAboutToChange);
        connect(mainWindow(), &MainWindow::projectChanged, m_statCollector.get(),
                &ProjectStatCollector::onProjectChanged);
        connect(mainWindow(), &MainWindow::projectChanged, [this] (ProjectModel* m) {
            m_openStatsAction->setEnabled(m != nullptr);
        });
        connect(m_openStatsAction.get(), &QAction::triggered, [this](bool) {
            if (m_statCollector->m_model) {
                StatsDialog dlg(m_statCollector->m_model);
                dlg.exec();
            }
        });
    }

    void StatsPlugin::unload()
    {
        m_statCollector.reset();
    }
}
