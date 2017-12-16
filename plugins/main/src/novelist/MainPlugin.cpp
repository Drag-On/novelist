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
#include <util/TranslationManager.h>
#include <QtCore/QLibraryInfo>
#include <QtWidgets/QMenuBar>
#include "MainPlugin.h"

namespace novelist {
    bool MainPlugin::load(gsl::not_null<Settings*> settings)
    {
        auto langDir = QDir(QApplication::applicationDirPath() + "/core");
        TranslationManager::instance().registerInDirectory(langDir, "novelist_core");
        auto mainLangDir = QDir(QApplication::applicationDirPath() + "/plugins/main");
        TranslationManager::instance().registerInDirectory(mainLangDir, "novelist_main");
        auto qtLangDir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
        TranslationManager::instance().registerInDirectory(qtLangDir, "qt", true);

        settings->registerPage(std::make_unique<SettingsPage_General_Creator>());
        settings->registerPage(std::make_unique<SettingsPage_Editor_Creator>());

        m_mainWindow = std::make_unique<MainWindow>();

        return true;
    }

    void MainPlugin::setup(QVector<PluginInfo> const& /* pluginInfo */)
    {
        auto helpMenu = m_mainWindow->findChild<QMenu*>("menu_Help");
        if (helpMenu != nullptr) {
            auto viewMenu = m_mainWindow->createPopupMenu();
            connect(m_mainWindow.get(), &MainWindow::languageChanged, [viewMenu] { viewMenu->setTitle(tr("View")); });
            viewMenu->setTitle(tr("View"));
            m_mainWindow->menuBar()->insertMenu(helpMenu->menuAction(), viewMenu);
        }
        else
            qWarning() << "Unable to create view menu";

        m_mainWindow->show();
    }

    void MainPlugin::unload()
    {
        m_mainWindow.reset();
    }
}
