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
#include "MainPlugin.h"

namespace novelist
{
    bool MainPlugin::load(gsl::not_null<Settings*> settings)
    {
        auto langDir = QDir(QApplication::applicationDirPath() + "/core");
        TranslationManager::instance().registerInDirectory(langDir, "novelist_core");
        auto qtLangDir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
        TranslationManager::instance().registerInDirectory(qtLangDir, "qt", true);

        settings->registerPage(std::make_unique<SettingsPage_General_Creator>());
        settings->registerPage(std::make_unique<SettingsPage_Editor_Creator>());

        m_mainWindow = std::make_unique<MainWindow>();

        return true;
    }

    void MainPlugin::setup(QVector<PluginInfo> const& /* pluginInfo */)
    {
        m_mainWindow->show();
    }

    void MainPlugin::unload()
    {
        m_mainWindow.reset();
    }
}
