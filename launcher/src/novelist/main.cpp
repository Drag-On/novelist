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
#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <fstream>

using namespace novelist;

std::string curTimePretty() noexcept {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::stringstream ss;
    ss.imbue(std::locale("en_US.utf8"));
    ss << std::put_time(&tm, "%c") << '\n';
    std::string time = ss.str();
    time = std::string(time.begin(), time.end() - 1); // Remove trailing newline
    return time;
}

std::string curTime() noexcept {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y%m%d_%H%M%S") << '\n';
    std::string time = ss.str();
    time = std::string(time.begin(), time.end() - 1); // Remove trailing newline
    return time;
}

void logMessage(QtMsgType type, QMessageLogContext const& context, QString const& msg)
{
    static std::ofstream log(QCoreApplication::applicationDirPath().toStdString() + "/logs/" + curTime() + ".log");

    QByteArray localMsg = msg.toLocal8Bit();
    std::string msgStr = std::string(localMsg.constData());
    if (context.file != nullptr && context.function != nullptr)
        msgStr += " (" + std::string(context.file) + ":" + std::to_string(context.line) + ", " + context.function + ")";
    msgStr += "\n";
    std::string typeStr;
    switch (type) {
        case QtDebugMsg:
            typeStr = "Debug";
            break;
        case QtInfoMsg:
            typeStr = "Info";
            break;
        case QtWarningMsg:
            typeStr = "Warning";
            break;
        case QtCriticalMsg:
            typeStr = "Critical";
            break;
        case QtFatalMsg:
            typeStr = "Fatal";
            break;
    }

    std::string time = curTimePretty();
    std::cerr << time << ": " << typeStr << ": " << msgStr;
    if (log.is_open())
        log << time << ": " << typeStr << ": " << msgStr;

    if (type == QtFatalMsg)
        std::terminate();
}

int main(int argc, char* argv[])
{
    qInstallMessageHandler(logMessage);

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
