/**********************************************************
 * @file   Settings.cpp
 * @author jan
 * @date   11/2/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <gsl/gsl>
#include "settings/Settings.h"

namespace novelist {
    void Settings::registerPage(std::unique_ptr<SettingsPage> page) noexcept
    {
        s_pages.push_back(std::move(page));
    }

    void Settings::updateAll() noexcept
    {
        QSettings settings;
        for (auto& s_page : Settings::s_pages) {
            settings.beginGroup(s_page->uid());
            s_page->initiateUpdate(settings);
            settings.endGroup();
        }
    }
}