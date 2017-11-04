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

    SettingsPage* Settings::findPage(QString const& uid) noexcept
    {
        auto iter = std::find_if(s_pages.begin(), s_pages.end(), [&uid] (auto const& p) { return p->uid() == uid; });
        if (iter != s_pages.end())
            return iter->get();
        return nullptr;
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