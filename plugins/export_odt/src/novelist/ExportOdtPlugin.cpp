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

        void handleError(zip_t* zip)
        {
            zip_error_t* error = zip_get_error(zip);
            auto cleanError = gsl::finally([error] { zip_error_fini(error); });
            char const* msg = zip_error_strerror(error);
            qWarning() << msg;
            notifyUserOnError(msg);
        }

        bool writeFile(zip_t* zip, std::string const& xml, std::string const& filename)
        {
            // meta.xml
            zip_source_t* fileSrc = zip_source_buffer(zip, xml.c_str(), xml.size(), 0);
            if (fileSrc == nullptr) {
                handleError(zip);
                return false;
            }
            zip_int64_t fileIdx = zip_file_add(zip, filename.c_str(), fileSrc, ZIP_FL_ENC_UTF_8);
            if (fileIdx < 0) {
                zip_source_free(fileSrc);
                handleError(zip);
                return false;
            }
            return true;
        }

        bool writeManifest(zip_t* zip, std::string const& xml)
        {
            // META-INF
            zip_int64_t dirIdx = zip_dir_add(zip, "META-INF", ZIP_FL_ENC_UTF_8);
            if (dirIdx < 0) {
                handleError(zip);
                return false;
            }
            // META-INF/manifest.xml
            return writeFile(zip, xml, "META-INF/manifest.xml");
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

            // Note: These strings have to outlive the zip pointer
            auto manifestXml = createManifest().toStdString();
            auto metaXml = createMeta(project).toStdString();
            auto mimeType = createMimeType().toStdString();
            auto stylesXml = createStyles(project).toStdString();

            int errCode = 0;
            zip_t* zip = zip_open(filename.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &errCode);
            if (errCode == 0) {
                auto closeZip = gsl::finally([zip] { zip_close(zip); });
                if (!writeManifest(zip, manifestXml))
                    return;
                if (!writeFile(zip, metaXml, "meta.xml"))
                    return;
                if (!writeFile(zip, mimeType, "mimetype"))
                    return;
                if (!writeFile(zip, stylesXml, "styles.xml"))
                    return;
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

    QString ExportOdtPlugin::createMeta(ProjectModel const* model) const noexcept
    {
        QString xml;
        QXmlStreamWriter xmlWriter(&xml);
        xmlWriter.setCodec("UTF-8");
        xmlWriter.setAutoFormatting(true);

        xmlWriter.writeStartDocument();

        xmlWriter.writeStartElement("office:document-meta");
        xmlWriter.writeAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
        xmlWriter.writeAttribute("xmlns:meta", "urn:oasis:names:tc:opendocument:xmlns:meta:1.0");
        xmlWriter.writeAttribute("xmlns:dc", "http://purl.org/dc/elements/1.1/");
        xmlWriter.writeAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");

        xmlWriter.writeStartElement("office:meta");

        xmlWriter.writeStartElement("meta:generator");
        xmlWriter.writeCharacters("Novelist");
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement("dc:creator");
        xmlWriter.writeCharacters(model->properties().m_author);
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement("dc:language");
        xmlWriter.writeCharacters(lang::identifier(model->properties().m_lang));
        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();

        xmlWriter.writeEndDocument();

        return xml;
    }

    QString ExportOdtPlugin::createMimeType() const noexcept
    {
        return "application/vnd.oasis.opendocument.text";
    }

    QString ExportOdtPlugin::createStyles(ProjectModel const* model) const noexcept
    {
        QString xml;
        QXmlStreamWriter xmlWriter(&xml);
        xmlWriter.setCodec("UTF-8");
        xmlWriter.setAutoFormatting(true);

        xmlWriter.writeStartDocument();

        xmlWriter.writeStartElement("office:document-styles");
        xmlWriter.writeAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
        xmlWriter.writeAttribute("xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
        xmlWriter.writeAttribute("xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0");
        xmlWriter.writeAttribute("xmlns:meta", "urn:oasis:names:tc:opendocument:xmlns:meta:1.0");

        xmlWriter.writeStartElement("office:styles");

        // Default style
        xmlWriter.writeStartElement("style:default-style");
        xmlWriter.writeAttribute("style:family", "paragraph");

        xmlWriter.writeStartElement("style:paragraph-properties");
        xmlWriter.writeAttribute("fo:orphans", "2");
        xmlWriter.writeAttribute("fo:widows", "2");
        xmlWriter.writeAttribute("fo:hyphenation-ladder-count", "no-limit");
        xmlWriter.writeAttribute("style:text-autospace", "ideograph-alpha");
        xmlWriter.writeAttribute("style:punctuation-wrap", "hanging");
        xmlWriter.writeAttribute("style:line-break", "strict");
        xmlWriter.writeAttribute("style:tab-stop-distance", "0.4925in");
        xmlWriter.writeAttribute("style:writing-mode", "page");
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement("style:text-properties");
        xmlWriter.writeAttribute("style:use-window-font-color", "true");
        xmlWriter.writeAttribute("style:font-name", "Liberation Serif"); // TODO: Adopt editor font
        xmlWriter.writeAttribute("fo:font-size", "12pt");
        xmlWriter.writeAttribute("fo:language", lang::languageCode(model->properties().m_lang));
        xmlWriter.writeAttribute("fo:country", lang::countryCode(model->properties().m_lang));
        xmlWriter.writeAttribute("style:letter-kerning", "true");
        xmlWriter.writeAttribute("fo:hyphenate", "true");
        xmlWriter.writeAttribute("fo:hyphenation-remain-char-count", "2");
        xmlWriter.writeAttribute("fo:hyphenation-push-char-count", "2");
        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();

        // Heading style
        xmlWriter.writeStartElement("style:style");
        xmlWriter.writeAttribute("style:name", "Heading");
        xmlWriter.writeAttribute("style:family", "paragraph");
        xmlWriter.writeAttribute("style:class", "text");

        xmlWriter.writeStartElement("style:paragraph-properties");
        xmlWriter.writeAttribute("fo:margin-top", "0.1665in");
        xmlWriter.writeAttribute("fo:margin-bottom", "0.0835in");
        xmlWriter.writeAttribute("loext:contextual-spacing", "false");
        xmlWriter.writeAttribute("fo:keep-with-next", "always");
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement("style:text-properties");
        xmlWriter.writeAttribute("style:font-name", "Liberation Serif"); // TODO: Adopt editor font
        xmlWriter.writeAttribute("style:font-family-generic", "swiss");
        xmlWriter.writeAttribute("style:font-pitch", "variable");
        xmlWriter.writeAttribute("fo:font-size", "14pt");
        xmlWriter.writeAttribute("fo:language", lang::languageCode(model->properties().m_lang));
        xmlWriter.writeAttribute("fo:country", lang::countryCode(model->properties().m_lang));
        xmlWriter.writeAttribute("style:letter-kerning", "true");
        xmlWriter.writeAttribute("fo:hyphenate", "true");
        xmlWriter.writeAttribute("fo:hyphenation-remain-char-count", "2");
        xmlWriter.writeAttribute("fo:hyphenation-push-char-count", "2");
        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();

        // Text body style
        xmlWriter.writeStartElement("style:style");
        xmlWriter.writeAttribute("style:name", "TextBody");
        xmlWriter.writeAttribute("style:display-name", "Text Body");
        xmlWriter.writeAttribute("style:family", "paragraph");
        xmlWriter.writeAttribute("style:class", "text");

        xmlWriter.writeStartElement("style:paragraph-properties");
        xmlWriter.writeAttribute("fo:margin-left", "0in");
        xmlWriter.writeAttribute("fo:margin-right", "0in");
        xmlWriter.writeAttribute("fo:margin-top", "0in");
        xmlWriter.writeAttribute("fo:margin-bottom", "0in");
        xmlWriter.writeAttribute("loext:contextual-spacing", "false");
        xmlWriter.writeAttribute("fo:line-height", "120%");
        xmlWriter.writeAttribute("fo:text-indent", "0.2in");
        xmlWriter.writeAttribute("style:auto-text-indent", "false");
        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();

        // Title style
        xmlWriter.writeStartElement("style:style");
        xmlWriter.writeAttribute("style:name", "Title");
        xmlWriter.writeAttribute("style:family", "paragraph");
        xmlWriter.writeAttribute("style:parent-style-name", "Heading");
        xmlWriter.writeAttribute("style:class", "chapter");

        xmlWriter.writeStartElement("style:paragraph-properties");
        xmlWriter.writeAttribute("fo:text-align", "center");
        xmlWriter.writeAttribute("style:justify-single-word", "false");
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement("style:text-properties");
        xmlWriter.writeAttribute("fo:font-size", "28pt");
        xmlWriter.writeAttribute("fo:font-weight", "bold");
        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();

        // Subtitle style
        xmlWriter.writeStartElement("style:style");
        xmlWriter.writeAttribute("style:name", "Subtitle");
        xmlWriter.writeAttribute("style:family", "paragraph");
        xmlWriter.writeAttribute("style:parent-style-name", "Heading");
        xmlWriter.writeAttribute("style:class", "chapter");

        xmlWriter.writeStartElement("style:paragraph-properties");
        xmlWriter.writeAttribute("fo:margin-top", "0.0417in");
        xmlWriter.writeAttribute("fo:margin-bottom", "0.0835in");
        xmlWriter.writeAttribute("loext:contextual-spacing", "false");
        xmlWriter.writeAttribute("fo:text-align", "center");
        xmlWriter.writeAttribute("style:justify-single-word", "false");
        xmlWriter.writeEndElement();

        xmlWriter.writeStartElement("style:text-properties");
        xmlWriter.writeAttribute("fo:font-size", "18pt");
        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();

        // Small caps style
        xmlWriter.writeStartElement("style:style");
        xmlWriter.writeAttribute("style:name", "SmallCaps");
        xmlWriter.writeAttribute("style:display-name", "Small Caps");
        xmlWriter.writeAttribute("style:family", "text");

        xmlWriter.writeStartElement("style:text-properties");
        xmlWriter.writeAttribute("fo:font-variant", "small-caps");
        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();

        // Page layout
        xmlWriter.writeStartElement("office:automatic-styles");

        xmlWriter.writeStartElement("style:page-layout");
        xmlWriter.writeAttribute("style:name", "PageLayout");

        xmlWriter.writeStartElement("style:page-layout-properties");
        xmlWriter.writeAttribute("fo:page-width", "8.2681in");
        xmlWriter.writeAttribute("fo:page-height", "11.6929in");
        xmlWriter.writeAttribute("style:num-format", "1");
        xmlWriter.writeAttribute("style:print-orientation", "portrait");
        xmlWriter.writeAttribute("fo:margin-top", "0.7874in");
        xmlWriter.writeAttribute("fo:margin-bottom", "0.7874in");
        xmlWriter.writeAttribute("fo:margin-left", "0.7874in");
        xmlWriter.writeAttribute("fo:margin-right", "0.7874in");
        xmlWriter.writeAttribute("style:writing-mode", "lr-tb");
        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();

        xmlWriter.writeEndDocument();

        return xml;
    }
}
