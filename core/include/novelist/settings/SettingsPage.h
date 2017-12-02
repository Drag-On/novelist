/**********************************************************
 * @file   SettingsPage.h
 * @author jan
 * @date   11/2/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_SETTINGSPAGE_H
#define NOVELIST_SETTINGSPAGE_H

#include <memory>
#include <QtWidgets/QWidget>
#include <QtCore/QSettings>
#include <novelist_core_export.h>

namespace novelist {
    /**
     * Manages a single page in the settings dialog and makes sure the dialog and the actual data stays in sync
     */
    class NOVELIST_CORE_EXPORT SettingsPage : public QObject {
    Q_OBJECT

    public:
        ~SettingsPage() override = default;

        /**
         * @return Display name of the page
         */
        virtual QString name() = 0;

        /**
         * @return A unique ID among all settings pages
         */
        virtual QString uid() = 0;

        /**
         * Initialize the widget based on the current settings
         * @param widget Widget, guaranteed to be identical in type to an object created by createWidget() earlier
         * @param settings Current settings of the page's group
         */
        virtual void initialize(QWidget* widget, QSettings const& settings) noexcept = 0;

        /**
         * Change current settings according to the modifications in the page widget
         * @param widget Widget, guaranteed to be identical in type to an object created by createWidget() earlier
         * @param settings Current settings of the page's group
         */
        virtual void apply(QWidget const* widget, QSettings& settings) noexcept = 0;

        /**
         * Initiate update of the application based on the changed settings
         * @param settings Current settings of the page's group
         */
        virtual void initiateUpdate(QSettings const& settings) noexcept = 0;

        /**
         * Restore default values on the widget
         * @param widget Widget, guaranteed to be identical in type to an object created by createWidget() earlier
         */
        virtual void restoreDefaults(QWidget const* widget) noexcept = 0;

        /**
         * Create a widget that is shown as the configuration page in the settings dialog
         * @return The widget to appear as a page
         */
        virtual QWidget* createWidget() noexcept = 0;

    signals:

        /**
         * Fired whenever the settings of this page are being updated
         */
        void updateInitiated();
    };
}

#endif //NOVELIST_SETTINGSPAGE_H
