/**********************************************************
 * @file   IPlugin.h
 * @author Jan Möller
 * @date   11.03.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#ifndef NOVELIST_ITESTINTERFACE_H
#define NOVELIST_ITESTINTERFACE_H

#include <QtCore/QObject>
#include <QtWidgets/QApplication>
#include "PluginInfo.h"

namespace novelist
{
    /**
     * Interface for plugins
     */
    class IPlugin
    {
    public:
        virtual ~IPlugin() = default;

        /**
         * Initialize the plugin.
         *
         * This is guaranteed to be called after all dependencies have been loaded. Therefore, this is the place
         * to hook into other plugins.
         * @return True in case the plugin could be loaded successfully, otherwise false
         */
        virtual bool load() = 0;

        /**
         * General set-up of the plugin.
         *
         * This will be called after all plugins have been loaded. This method should not be used to create additional
         * GUI elements, since doing so will prevent other plugins from hooking in.
         * @param pluginInfo Information about other registered plugins.
         */
        virtual void setup(QVector<PluginInfo> const& pluginInfo) = 0;

        /**
         * Unload the plugin.
         *
         * This will be called before any dependencies are unloaded.
         */
        virtual void unload() = 0;
    };
}

Q_DECLARE_INTERFACE(novelist::IPlugin, "novelist.IPlugin")

#endif //NOVELIST_ITESTINTERFACE_H
