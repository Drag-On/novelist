/**********************************************************
 * @file   StatsPlugin.cpp
 * @author Jan Möller
 * @date   19.11.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <gsl/gsl>
#include <QtWidgets/QAction>
#include "ExportOdtPlugin.h"

namespace novelist {
    bool ExportOdtPlugin::load(gsl::not_null<Settings*> settings)
    {
        m_action = std::make_unique<QAction>(tr("OpenDocument (*.odt)"));

        return ExporterPlugin::load(settings);
    }

    gsl::not_null<QAction*> novelist::ExportOdtPlugin::exportAction() const noexcept
    {
        return m_action.get();
    }
}
