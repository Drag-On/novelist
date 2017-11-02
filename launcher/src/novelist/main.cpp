/**********************************************************
 * @file   main.cpp
 * @author Jan Möller
 * @date   11.03.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <QApplication>
#include <QMessageBox>
#include <settings/Settings.h>
#include <plugin/Plugin.h>
#include <plugin/PluginManager.h>

using namespace novelist;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationDisplayName("Novelist");
    QApplication::setApplicationName("Novelist");
    QApplication::setOrganizationName("Novelist-Org");
    QApplication::setOrganizationDomain("novelist-nodomain.org");

    int retCode = 0;

    Settings settings;
    auto const settingsFile = settings.fileName();
    auto const pluginDir = QDir(app.applicationDirPath() + "/" + pluginSubDir);
    PluginManager mngr(&settings, pluginDir);
    mngr.scan();
    if (mngr.load() > 0)
        retCode = app.exec();
    else {
        QMessageBox msgBox;
        msgBox.setWindowTitle(QCoreApplication::translate("main", "Error"));
        msgBox.setText(
                QCoreApplication::translate("main", "Plugins could not be found. Application is going to exit."));
        msgBox.setInformativeText(QCoreApplication::translate("main",
                R"(Either the directory "%1" does not contain any plugins, or they are disabled in settings at "%2".)").arg(
                pluginDir.path()).arg(settingsFile));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
    }

    mngr.unload();

    return retCode;
}
