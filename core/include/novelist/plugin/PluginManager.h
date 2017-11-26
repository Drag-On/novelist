/**********************************************************
 * @file   PluginManager.h
 * @author Jan Möller
 * @date   11.03.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#ifndef NOVELIST_PLUGINMANAGER_H
#define NOVELIST_PLUGINMANAGER_H

#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QPluginLoader>
#include <QtCore/QSettings>
#include <QtCore/QVector>
#include <gsl/gsl>
#include "settings/Settings.h"
#include <novelist_core_export.h>

namespace novelist
{
    /**
     * The plugin manager tracks plugins, checks if their requirements are met and loads them in an appropriate order.
     */
    class NOVELIST_CORE_EXPORT PluginManager
    {
    public:
        /**
         * Constructor
         * @param settings Settings object
         * @param pluginPath Folder where plugins are located
         */
        PluginManager(gsl::not_null<Settings*> settings, QDir const& pluginPath);

        /**
         * Scans the plugin folder for plugins.
         *
         * This will sanity-check all found plugins and disable any plugins whose requirements are not met. The load
         * order is determined based on the dependency graph, i.e. plugins will always be loaded after the plugins they
         * depend on. In the case of circular dependencies some plugins will not be loaded, however they will not be
         * disabled. An appropriate error message is written to qWarning().
         */
        void scan();

        /**
         * Loads all enabled plugins in the order determined by scan().
         * @return Amount of loaded plugins
         */
        int load();

        /**
         * Unloads all plugins in reverse order of loading.
         */
        void unload();

    private:
        struct PluginData
        {
            size_t infoIdx;         ///< Index into m_pluginInfo
            QPluginLoader* pLoader; ///< Loader object
        };

        gsl::not_null<Settings*> m_settings;
        QDir m_pluginDir;
        QHash<QString, PluginData> m_plugins;
        QList<QString> m_loadOrder;
        QVector<PluginInfo> m_pluginInfo;
        static QString const s_pluginsSettingsGroup;

        void searchPluginDir();

        void checkAll();

        void determineLoadOrder();

        QMultiHash<QString, QString> determineDependencyGraph();

        bool check(QString uid);

        bool tryLoad(QString uid);

        bool visitNode(QString const& n, QHash<QString, int>& marks, QMultiHash<QString, QString> const& depGraph);

        void storeSettings();
    };
}

#endif //NOVELIST_PLUGINMANAGER_H
