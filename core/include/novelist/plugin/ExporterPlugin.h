/**********************************************************
 * @file   ExporterPlugin.h
 * @author jan
 * @date   12/3/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_EXPORTERPLUGIN_H
#define NOVELIST_EXPORTERPLUGIN_H

#include "BasePlugin.h"

namespace novelist {
    /**
     * Abstract base class for plugins that implement a project exporter
     */
    class NOVELIST_CORE_EXPORT ExporterPlugin : public BasePlugin {
    Q_OBJECT

    public:
        bool load(gsl::not_null<Settings*> settings) override;

        void setup(QVector<PluginInfo> const& pluginInfo) override;

        void unload() override;

        /**
         * Provides access to the current project. Only call this when the export action was triggered!
         * @return Pointer to the current project
         */
        gsl::not_null<ProjectModel const*> curProject() const noexcept;

        /**
         * @return Action that should be shown in the file/export menu
         */
        virtual gsl::not_null<QAction*> exportAction() const noexcept = 0;
    };
}

#endif //NOVELIST_EXPORTERPLUGIN_H
