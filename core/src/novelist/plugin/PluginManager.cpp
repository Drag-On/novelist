/**********************************************************
 * @file   PluginManager.cpp
 * @author Jan Möller
 * @date   11.03.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <QtCore>
#include <gsl/gsl>
#include <QtWidgets/QMessageBox>
#include "plugin/Plugin.h"
#include "plugin/PluginManager.h"

namespace novelist {
    PluginManager::PluginManager(gsl::not_null<Settings*> settings, QDir const& pluginPath)
            :m_settings(settings),
             m_pluginDir(pluginPath)
    {
    }

    void PluginManager::scan()
    {
        // Scan plugin directory
        searchPluginDir();

        // Plugin sanity check
        checkAll();

        // Store enabled/disabled
        storeSettings();

        // Determine load order
        determineLoadOrder();
    }

    void PluginManager::determineLoadOrder()
    {
        QMultiHash<QString, QString> depGraph = determineDependencyGraph();
        m_loadOrder.clear();
        QHash<QString, int> marks;
        for (QString const& p : m_plugins.keys())
            if (m_pluginInfo[m_plugins[p].infoIdx].enabled)
                marks.insert(p, 0);

        // While there are unmarked nodes
        while (!marks.keys(0).empty()) {
            QString node = marks.keys(0).front();
            if (!visitNode(node, marks, depGraph)) {
                qWarning() << "Plugin dependencies have cycles.";
                break;
            }
        }
    }

    int PluginManager::load()
    {
        int numLoaded = 0;
        for (QString const& p : m_loadOrder) {
            qInfo() << "Loading plugin" << p;
            if (!tryLoad(p))
                qWarning() << "Plugin" << p << "was not loaded properly.";
            else numLoaded++;
        }

        Settings::updateAll();

        for (auto& p : m_plugins.values())
            if (p.pLoader->isLoaded()) {
                novelist::Plugin* pPlugin = qobject_cast<Plugin*>(p.pLoader->instance());
                if (pPlugin)
                    pPlugin->setup(m_pluginInfo);
            }

        return numLoaded;
    }

    void PluginManager::unload()
    {
        // Unload in opposite load order
        QList<QString>::const_iterator iter = m_loadOrder.constEnd();
        while (iter != m_loadOrder.constBegin()) {
            --iter;
            auto loader = m_plugins.find(*iter)->pLoader;
            if (loader->isLoaded()) {
                qInfo() << "Unloading plugin" << *iter;
                novelist::Plugin* pPlugin = qobject_cast<Plugin*>(loader->instance());
                if (pPlugin != nullptr)
                    pPlugin->unload();
                loader->unload();
                delete loader;
            }
        }
    }

    QMultiHash<QString, QString> PluginManager::determineDependencyGraph()
    {
        QMultiHash<QString, QString> dependencyGraph;

        for (PluginData& p : m_plugins) {
            if (!m_pluginInfo[p.infoIdx].enabled)
                continue;

            // Insert dependencies into graph
            for (QVariant dependency : m_pluginInfo[p.infoIdx].dependencies) {
                QVariantMap dependencyMap = dependency.toMap();
                QVariant depUid = dependencyMap.value("uid");
                dependencyGraph.insert(m_pluginInfo[p.infoIdx].uid.toString(), depUid.toString());
            }
        }

        return dependencyGraph;
    }

    void PluginManager::checkAll()
    {
        for (auto const& p : m_plugins)
            check(m_pluginInfo[p.infoIdx].uid.toString());
    }

    bool PluginManager::check(QString uid)
    {
        PluginData& p = *m_plugins.find(uid);
        PluginInfo& info = m_pluginInfo[p.infoIdx];

        auto const settingsKey = s_pluginsSettingsGroup + "/" + uid;
        if (!m_settings->contains(settingsKey)) {
            QMessageBox msgBox;
            msgBox.setWindowTitle("Unknown plugin");
            msgBox.setText("The plugin \"" % uid % "\" has never been loaded before. Should it be loaded?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setIcon(QMessageBox::Question);
            msgBox.setDefaultButton(QMessageBox::No);
            m_settings->setValue(settingsKey, msgBox.exec() == QMessageBox::Yes);
        }
        info.enabled = m_settings->value(settingsKey).toBool();

        if (!info.enabled)
            return true;

        // Check dependencies
        for (QVariant const& dependency : info.dependencies) {
            QVariantMap dependencyMap = dependency.toMap();
            QVariant depUid = dependencyMap.value("uid");
            QVariant depVer = dependencyMap.value("version");
            if (!m_plugins.contains(depUid.toString())) {
                qWarning() << "Disabling plugin" << info.uid.toString() << ". Missing dependency" << depUid.toString()
                           << ".";
                info.enabled = false;
                return false;
            }
            PluginData const& pDep = m_plugins.value(depUid.toString());
            if (m_pluginInfo[pDep.infoIdx].version != depVer) {
                qWarning() << "Disabling plugin" << info.uid.toString() << ". Version mismatch with dependency"
                           << depUid.toString() << ".";
                info.enabled = false;
                return false;
            }
        }

        return true;
    }

    bool PluginManager::tryLoad(QString uid)
    {
        if (!m_plugins.contains(uid)) {
            qWarning() << "Plugin" << uid << "could not be found.";
            return false;
        }

        PluginData& plugin = *m_plugins.find(uid);
        if (!plugin.pLoader->load()) {
            qWarning() << "Plugin" << uid << "(" << plugin.pLoader->fileName() << ") could not be loaded."
                       << plugin.pLoader->errorString();
            return false;
        }
        novelist::Plugin* pPlugin = qobject_cast<Plugin*>(plugin.pLoader->instance());
        if (pPlugin == nullptr) {
            qWarning() << "Plugin" << uid << "(" << plugin.pLoader->fileName() << ") is corrupted."
                       << plugin.pLoader->errorString();
            plugin.pLoader->unload();
            return false;
        }

        if (!pPlugin->load(m_settings)) {
            qWarning() << "Failed to load plugin" << uid << "(" << plugin.pLoader->fileName() << ").";
            plugin.pLoader->unload();
            return false;
        }

        m_pluginInfo[plugin.infoIdx].loaded = true;

        return true;
    }

    void PluginManager::searchPluginDir()
    {
        for (QFileInfo const& file : m_pluginDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot)) {
            PluginData pluginData{};
            QString filename = file.absoluteFilePath();
            if (!QLibrary::isLibrary(filename))
                continue;

            size_t infoId = static_cast<size_t>(m_pluginInfo.size());
            PluginInfo info;

            pluginData.pLoader = new QPluginLoader(filename);
            pluginData.infoIdx = infoId;

            info.uid = pluginData.pLoader->metaData().value("MetaData").toObject().value("uid").toVariant();
            info.version = pluginData.pLoader->metaData().value("MetaData").toObject().value("version").toVariant();
            info.dependencies = pluginData.pLoader->metaData().value("MetaData").toObject().value(
                    "dependencies").toArray().toVariantList();
            info.name = pluginData.pLoader->metaData().value("MetaData").toObject().value("name").toVariant();
            info.description = pluginData.pLoader->metaData().value("MetaData").toObject().value(
                    "description").toVariant();

            // Only add plugins if their uid is actually unique
            if (m_plugins.contains(info.uid.toString())) {
                qWarning() << "Plugin uid" << info.uid.toString() << "is not unique.";
                continue;
            }
            m_plugins.insert(info.uid.toString(), pluginData);
            m_pluginInfo.push_back(info);
        }
    }

    bool
    PluginManager::visitNode(QString const& n, QHash<QString, int>& marks, QMultiHash<QString, QString> const& depGraph)
    {
        if (marks.value(n) == 1) // temp mark
            return false;
        if (marks.value(n) == 0) // no mark
        {
            marks[n] = 1;
            for (QString const& m : depGraph.keys(n)) {
                bool success = visitNode(m, marks, depGraph);
                if (!success)
                    return false;
            }
            marks[n] = 2;
            m_loadOrder.push_front(n);
        }
        return true;
    }

    void PluginManager::storeSettings()
    {
        m_settings->beginGroup(s_pluginsSettingsGroup);
        auto endGroup = gsl::finally([this] { m_settings->endGroup(); });
        for (auto const& p : m_plugins)
            m_settings->setValue(m_pluginInfo[p.infoIdx].uid.toString(), m_pluginInfo[p.infoIdx].enabled);
        m_settings->sync();
    }
}