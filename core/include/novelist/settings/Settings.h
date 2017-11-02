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

namespace novelist {
    class SettingsWindow;

    /**
     * Application-wide settings
     */
    class Settings : public QSettings {
    public:
        /**
         * Register a page in the settings dialog
         * @param page Page to register
         */
        static void registerPage(std::unique_ptr<SettingsPage> page) noexcept;

    private:
        static inline std::vector<std::unique_ptr<SettingsPage>> s_pages;

        friend SettingsWindow;
    };
}

#endif //NOVELIST_SETTINGS_H
