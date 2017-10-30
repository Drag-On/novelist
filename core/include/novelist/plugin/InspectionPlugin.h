/**********************************************************
 * @file   InspectionPlugin.h
 * @author jan
 * @date   10/30/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_INSPECTIONPLUGIN_H
#define NOVELIST_INSPECTIONPLUGIN_H

#include <widgets/texteditor/Inspector.h>
#include "Plugin.h"

namespace novelist {
    /**
     * Abstract base class for plugins that want to provide an inspector. It will automatically be registered and used
     * in all scene editors.
     */
    class InspectionPlugin : public QObject, public Plugin {
    Q_OBJECT
    Q_INTERFACES(novelist::Plugin)

    public:
        bool load() override;

        void unload() override;

        void setup(QVector<PluginInfo> const& pluginInfo) override;

    protected:
        /**
         * @return New instance of the inspector
         */
        virtual std::unique_ptr<Inspector> createInspector() const noexcept = 0;
    };
}

#endif //NOVELIST_INSPECTIONPLUGIN_H
