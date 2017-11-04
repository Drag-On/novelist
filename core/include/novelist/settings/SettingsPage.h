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

namespace novelist {
    /**
     * Manages a single page in the settings dialog and makes sure the dialog and the actual data stays in sync
     */
    class SettingsPage {
    public:
        virtual ~SettingsPage() = default;

        /**
         * @return Display name of the page
         */
        virtual QString name() = 0;

        /**
         * @return A unique ID among all settings pages
         */
        virtual QString uid() = 0;

        /**
         * Update the widget based on the current settings
         * @param widget Widget, guaranteed to be identical in type to an object created by createWidget() earlier
         * @param settings Current settings
         */
        virtual void update(QWidget* widget, QSettings const& settings) noexcept = 0;

        /**
         * Change current settings according to the modifications in the page widget
         * @param widget Widget, guaranteed to be identical in type to an object created by createWidget() earlier
         * @param settings Current settings
         */
        virtual void apply(QWidget const* widget, QSettings& settings) noexcept = 0;

        /**
         * Restore default values on the widget
         * @param widget Widget, guaranteed to be identical in type to an object created by createWidget() earlier
         */
        virtual void restoreDefaults(QWidget const* widget) = 0;

        /**
         * Create a widget that is shown as the configuration page in the settings dialog
         * @return The widget to appear as a page
         */
        virtual QWidget* createWidget() noexcept = 0;
    };
}

#endif //NOVELIST_SETTINGSPAGE_H
