/**********************************************************
 * @file   PluginInfo.h
 * @author Jan Möller
 * @date   15.03.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#ifndef NOVELIST_PLUGININFO_H
#define NOVELIST_PLUGININFO_H

#include <QtCore/QVariantList>
#include <novelist_core_export.h>

namespace novelist
{
    /**
     * Stores meta information about a plugin
     */
    struct NOVELIST_CORE_EXPORT PluginInfo
    {
        QVariant uid;               ///< Unique identifier
        QVariant version;           ///< Version number
        QVariantList dependencies;  ///< List of dependencies, each of which has a uid and a version
        bool enabled = false;       ///< Indicates whether the plugin is enabled or not
        bool loaded = false;        ///< Indicates whether the plugin is currently loaded
        QVariant name;              ///< Human-friendly name
        QVariant description;       ///< Human-friendly description
    };

    /**
     * Plugin config file name
     */
    QString const pluginSettingsFile = QString("plugins.ini");

    /**
     * Plugin directory name
     */
    QString const pluginSubDir = QString("plugins");
}

#endif //NOVELIST_PLUGININFO_H
