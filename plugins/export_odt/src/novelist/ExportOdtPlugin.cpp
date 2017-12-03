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

            auto manifestXml = createManifest().toStdString();

            int errCode = 0;
            zip_t* zip = zip_open(filename.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &errCode);
            if (errCode == 0) {
                auto closeZip = gsl::finally([zip] { zip_close(zip); });

                // META-INF
                zip_int64_t dirIdx = zip_dir_add(zip, "META-INF", ZIP_FL_ENC_UTF_8);
                if (dirIdx < 0) {
                    handleError(zip);
                    return;
                }
                // META-INF/manifest.xml
                zip_source_t* fileSrc = zip_source_buffer(zip, manifestXml.c_str(), manifestXml.size(), 0);
                if (fileSrc == nullptr) {
                    handleError(zip);
                    return;
                }
                zip_int64_t fileIdx = zip_file_add(zip, "META-INF/manifest.xml", fileSrc, ZIP_FL_ENC_UTF_8);
                if (fileIdx < 0) {
                    zip_source_free(fileSrc);
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

    QString ExportOdtPlugin::createManifest() const noexcept
    {
        QString xml;
        QXmlStreamWriter xmlWriter(&xml);
        xmlWriter.setCodec("UTF-8");
        xmlWriter.setAutoFormatting(true);

        xmlWriter.writeStartDocument();

        xmlWriter.writeStartElement("manifest:manifest");
        xmlWriter.writeAttribute("xmlns:manifest", "urn:oasis:names:tc:opendocument:xmlns:manifest:1.0");
        xmlWriter.writeAttribute("manifest:version", "1.2");

        xmlWriter.writeStartElement("manifest:file-entry");
        xmlWriter.writeAttribute("manifest:full-path", "/");
        xmlWriter.writeAttribute("manifest:version", "1.2");
        xmlWriter.writeAttribute("manifest:media-type", "application/vnd.oasis.opendocument.text");
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement("manifest:file-entry");
        xmlWriter.writeAttribute("manifest:full-path", "content.xml");
        xmlWriter.writeAttribute("manifest:media-type", "text/xml");
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement("manifest:file-entry");
        xmlWriter.writeAttribute("manifest:full-path", "styles.xml");
        xmlWriter.writeAttribute("manifest:media-type", "text/xml");
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement("manifest:file-entry");
        xmlWriter.writeAttribute("manifest:full-path", "meta.xml");
        xmlWriter.writeAttribute("manifest:media-type", "text/xml");
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement("manifest:file-entry");
        xmlWriter.writeAttribute("manifest:full-path", "settings.xml");
        xmlWriter.writeAttribute("manifest:media-type", "text/xml");
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement("manifest:file-entry");
        xmlWriter.writeAttribute("manifest:full-path", "manifest.rdf");
        xmlWriter.writeAttribute("manifest:media-type", "application/rdf+xml");
        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();

        xmlWriter.writeEndDocument();

        return xml;
    }
}
