/**********************************************************
 * @file   LanguageToolPlugin.cpp
 * @author Jan Möller
 * @date   30.11.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include "LanguageToolInspector.h"
#include "LanguageToolPlugin.h"

namespace novelist
{
    std::unique_ptr<Inspector> novelist::LanguageToolPlugin::createInspector() const noexcept
    {
        return std::make_unique<LanguageToolInspector>();
    }
}
