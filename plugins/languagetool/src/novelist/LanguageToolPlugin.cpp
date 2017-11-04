/**********************************************************
 * @file   LanguageToolPlugin.cpp
 * @author Jan Möller
 * @date   30.11.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include "SettingsPage_LanguageTool.h"
#include "LanguageToolInspector.h"
#include "LanguageToolPlugin.h"

namespace novelist
{
    bool LanguageToolPlugin::load(gsl::not_null<Settings*> settings)
    {
        settings->registerPage(std::make_unique<SettingsPage_LanguageTool_Creator>());

        return InspectionPlugin::load(settings);
    }

    std::unique_ptr<Inspector> novelist::LanguageToolPlugin::createInspector() const noexcept
    {
        return std::make_unique<LanguageToolInspector>();
    }
}
