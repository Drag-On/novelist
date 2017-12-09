/**********************************************************
 * @file   BasePlugin.h
 * @author jan
 * @date   11/19/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_BASEPLUGIN_H
#define NOVELIST_BASEPLUGIN_H

#include <QObject>
#include "windows/MainWindow.h"
#include "Plugin.h"

namespace novelist {
    /**
     * Abstract base class for plugins that require access to the main window. This assumes that the plugin has
     * novelist_main set as a dependency.
     */
    class NOVELIST_CORE_EXPORT BasePlugin : public QObject, public Plugin {
    Q_OBJECT
        Q_INTERFACES(novelist::Plugin)

    public:
        bool load(gsl::not_null<Settings*> settings) override;

        /**
         * @return Pointer to the main window.
         * @note Might be nullptr if the main window couldn't be found. This typically indicates that the plugin doesn't
         *       have novelist_main set as its dependency.
         */
        MainWindow* mainWindow() noexcept;

        /**
         * @return Pointer to the main window.
         * @note Might be nullptr if the main window couldn't be found. This typically indicates that the plugin doesn't
         *       have novelist_main set as its dependency.
         */
        MainWindow const* mainWindow() const noexcept;

    private:
        MainWindow* m_mainWnd = nullptr;
    };
}

#endif //NOVELIST_BASEPLUGIN_H
