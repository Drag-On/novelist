/**********************************************************
 * @file   LanguageToolPlugin.cpp
 * @author Jan Möller
 * @date   30.11.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#ifndef NOVELIST_LANGUAGETOOLPLUGIN_H
#define NOVELIST_LANGUAGETOOLPLUGIN_H

#include <memory>
#include <QtCore/QObject>
#include <plugin/InspectionPlugin.h>

namespace novelist
{
    /**
     * Plugin that provides LanguageTool integration. It provides an inspector that relays all requests to a local
     * instance of the LanguageTool server.
     */
    class LanguageToolPlugin : public InspectionPlugin
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "novelist.LanguageToolPlugin" FILE "LanguageToolPlugin.json")

    public:
        bool load(gsl::not_null<Settings*> settings) override;

    protected:
        std::unique_ptr<Inspector> createInspector() const noexcept override;
    };
}

#endif //NOVELIST_LANGUAGETOOLPLUGIN_H
