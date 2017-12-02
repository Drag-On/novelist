/**********************************************************
 * @file   Settings.h
 * @author jan
 * @date   11/2/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_SETTINGS_H
#define NOVELIST_SETTINGS_H

#include <QtCore/QSettings>
#include "SettingsPage.h"
#include <novelist_core_export.h>

namespace novelist {
    class SettingsWindow;

    /**
     * Application-wide settings
     */
    class NOVELIST_CORE_EXPORT Settings : public QSettings {
    public:
        /**
         * Register a page in the settings dialog
         * @param page Page to register
         */
        NOVELIST_CORE_EXPORT static void registerPage(std::unique_ptr<SettingsPage> page) noexcept;

        /**
         * Looks for the page with a given Id.
         * @param uid Id to look for
         * @return A non-owning pointer to the found page or nullptr
         */
        NOVELIST_CORE_EXPORT static SettingsPage* findPage(QString const& uid) noexcept;

        /**
         * Updates the application based on all current settings
         */
        NOVELIST_CORE_EXPORT static void updateAll() noexcept;

    private:
        static std::vector<std::unique_ptr<SettingsPage>> s_pages;

        friend SettingsWindow;
    };
}

#endif //NOVELIST_SETTINGS_H
