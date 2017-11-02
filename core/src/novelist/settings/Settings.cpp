/**********************************************************
 * @file   Settings.cpp
 * @author jan
 * @date   11/2/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "settings/Settings.h"

namespace novelist {
    void Settings::registerPage(std::unique_ptr<SettingsPage> page) noexcept
    {
        s_pages.push_back(std::move(page));
    }
}