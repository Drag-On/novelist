/**********************************************************
 * @file   StatsPlugin.cpp
 * @author Jan Möller
 * @date   19.11.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#ifndef NOVELIST_EXPORTODTPLUGIN_H
#define NOVELIST_EXPORTODTPLUGIN_H

#include <plugin/ExporterPlugin.h>

namespace novelist
{
    class ExportOdtPlugin : public ExporterPlugin
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "novelist.ExportOdtPlugin" FILE "ExportOdtPlugin.json")

    public:
        bool load(gsl::not_null<Settings*> settings) override;

        gsl::not_null<QAction*> exportAction() const noexcept override;

    private:
        std::unique_ptr<QAction> m_action;
    };
}

#endif //NOVELIST_EXPORTODTPLUGIN_H
