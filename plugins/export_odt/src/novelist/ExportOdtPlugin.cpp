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
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

extern "C"
{
#include <zip.h>
}

#include "ExportOdtPlugin.h"

namespace novelist {
    namespace {
        void notifyUserOnError(std::string const& msg = "")
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle(QApplication::translate("ExportOdtPlugin", "Novelist"));
            msgBox.setText(QApplication::translate("ExportOdtPlugin", "Export failed."));
            msgBox.setInformativeText(QApplication::translate("ExportOdtPlugin", msg.c_str()));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
        }

        bool handleError(zip_t* zip)
        {
            zip_error_t* error = zip_get_error(zip);
            auto cleanError = gsl::finally([error] { zip_error_fini(error); });
            char const* msg = zip_error_strerror(error);
            qWarning() << msg;
            notifyUserOnError(msg);
        }
    }

    bool ExportOdtPlugin::load(gsl::not_null<Settings*> settings)
    {
        m_action = std::make_unique<QAction>(tr("OpenDocument (*.odt)"));
        connect(m_action.get(), &QAction::triggered, this, &ExportOdtPlugin::onExport);

        return ExporterPlugin::load(settings);
    }

    gsl::not_null<QAction*> novelist::ExportOdtPlugin::exportAction() const noexcept
    {
        return m_action.get();
    }

    void ExportOdtPlugin::onExport() const noexcept
    {
        auto project = curProject();
        QFileDialog dialog;
        dialog.setFileMode(QFileDialog::AnyFile);
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        dialog.setDefaultSuffix("odt");
        dialog.setNameFilter("*.odt");
        if (dialog.exec() == QFileDialog::Accepted) {
            auto filename = dialog.selectedFiles().first().toStdString();
            qInfo() << "Export to" << filename.c_str();

            int errCode = 0;
            zip_t* zip = zip_open(filename.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &errCode);
            if (errCode == 0) {
                auto closeZip = gsl::finally([zip] { zip_close(zip); });

                zip_int64_t idx = zip_dir_add(zip, "META-INF", ZIP_FL_ENC_UTF_8);
                if (idx < 0) {
                    handleError(zip);
                    return;
                }
            }
            else {
                zip_error_t error{};
                zip_error_init_with_code(&error, errCode);
                auto cleanError = gsl::finally([&error] { zip_error_fini(&error); });
                char const* msg = zip_error_strerror(&error);

                qWarning() << msg;
                notifyUserOnError(msg);
            }
        }
    }
}
