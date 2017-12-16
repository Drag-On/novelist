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
#include <util/Overloaded.h>
#include <util/TranslationManager.h>

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

        int maxHeadingDepth(ProjectModel const* model, QModelIndex idx, int maxDepth = 0)
        {
            for (int i = 0; i < model->rowCount(idx); ++i) {
                auto const& child = idx.child(i, 0);
                if (model->nodeType(child) == ProjectModel::NodeType::Chapter) {
                    ++maxDepth;
                    maxDepth = std::max(maxHeadingDepth(model, child, maxDepth), maxDepth);
                }
            }
            return maxDepth;
        }

        void writeChapterXml(QXmlStreamWriter& xml, ProjectModel const* model, QModelIndex idx, int depth = 1)
        {
            int consecutiveSceneCount = 0;
            for (int i = 0; i < model->rowCount(idx); ++i) {
                auto const& child = idx.child(i, 0);
                auto const& data = model->nodeData(child);
                std::visit(Overloaded {
                        [](auto&) { },
                        [&xml, &consecutiveSceneCount](ProjectModel::SceneData& arg) {
                            ++consecutiveSceneCount;

                            for (auto b = arg.m_doc->begin(); b != arg.m_doc->end(); b = b.next()) {
                                if (b.isValid()) {
                                    xml.writeStartElement("text:p");
                                    if (b.blockNumber() == 0 && consecutiveSceneCount == 1)
                                        xml.writeAttribute("text:style-name", "par_no_indent");
                                    else if (b.blockNumber() == 0 && consecutiveSceneCount > 1)
                                        xml.writeAttribute("text:style-name", "par_long_skip");
                                    else
                                        xml.writeAttribute("text:style-name", "TextBody");
                                    for (auto iter = b.begin(); iter != b.end(); ++iter) {
                                        QTextFragment currentFragment = iter.fragment();
                                        if (currentFragment.isValid()) {
                                            auto const& format = currentFragment.charFormat();
                                            int toClose = 0;
                                            if (format.font().weight() > QFont::Weight::Normal)
                                            {
                                                xml.writeStartElement("text:span");
                                                xml.writeAttribute("text:style-name", "Bold");
                                                ++toClose;
                                            }
                                            if (format.font().italic())
                                            {
                                                xml.writeStartElement("text:span");
                                                xml.writeAttribute("text:style-name", "Italic");
                                                ++toClose;
                                            }
                                            if (format.font().underline())
                                            {
                                                xml.writeStartElement("text:span");
                                                xml.writeAttribute("text:style-name", "Underline");
                                                ++toClose;
                                            }
                                            if (format.font().overline())
                                            {
                                                xml.writeStartElement("text:span");
                                                xml.writeAttribute("text:style-name", "Overline");
                                                ++toClose;
                                            }
                                            if (format.font().strikeOut())
                                            {
                                                xml.writeStartElement("text:span");
                                                xml.writeAttribute("text:style-name", "Strikethrough");
                                                ++toClose;
                                            }
                                            if (format.font().capitalization() == QFont::Capitalization::SmallCaps)
                                            {
                                                xml.writeStartElement("text:span");
                                                xml.writeAttribute("text:style-name", "SmallCaps");
                                                ++toClose;
                                            }
                                            xml.writeCharacters(currentFragment.text());

                                            for (int i = 0; i < toClose; ++i)
                                                xml.writeEndElement();
                                        }
                                    }
                                    xml.writeEndElement();
                                }
                            }
                        },
                        [&xml, model, &child, &consecutiveSceneCount, depth](ProjectModel::ChapterData& arg) {
                            consecutiveSceneCount = 0;
                            xml.writeStartElement("text:p");
                            xml.writeAttribute("text:style-name", "Heading" + QString::number(depth));
                            xml.writeCharacters(arg.m_name);
                            xml.writeEndElement();

                            writeChapterXml(xml, model, child, depth + 1);
                        },
                }, *data);
            }
        }
    }

    bool ExportOdtPlugin::load(gsl::not_null<Settings*> settings)
    {
        auto langDir = QDir(QApplication::applicationDirPath() + "/plugins/export_odt");
        TranslationManager::instance().registerInDirectory(langDir, "novelist_export_odt");

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
            auto manifestRdf = createManifestRdf().toStdString();
            auto metaXml = createMeta(project).toStdString();
            auto mimeType = createMimeType().toStdString();
            auto settingsXml = createSettings().toStdString();
            auto stylesXml = createStyles(project).toStdString();
            auto contentXml = createContent(project).toStdString();

            int errCode = 0;
            zip_t* zip = zip_open(filename.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &errCode);
            if (errCode == 0) {
                auto closeZip = gsl::finally([zip] { zip_close(zip); });
                if (!writeManifest(zip, manifestXml))
                    return;
                if (!writeFile(zip, manifestRdf, "manifest.rdf"))
                    return;
                if (!writeFile(zip, metaXml, "meta.xml"))
                    return;
                if (!writeFile(zip, mimeType, "mimetype"))
                    return;
                if (!writeFile(zip, settingsXml, "settings.xml"))
                    return;
                if (!writeFile(zip, stylesXml, "styles.xml"))
                    return;
                if (!writeFile(zip, contentXml, "content.xml"))
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
        xmlWriter.setAutoFormatting(false);

        xmlWriter.writeStartDocument();

        xmlWriter.writeStartElement("manifest:manifest");
        xmlWriter.writeAttribute("xmlns:manifest", "urn:oasis:names:tc:opendocument:xmlns:manifest:1.0");
        xmlWriter.writeAttribute("manifest:version", "1.2");

        {
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
        }

        xmlWriter.writeEndElement();

        xmlWriter.writeEndDocument();

        return xml;
    }

    QString ExportOdtPlugin::createManifestRdf() const noexcept
    {
        QString xml;
        QXmlStreamWriter xmlWriter(&xml);
        xmlWriter.setCodec("UTF-8");
        xmlWriter.setAutoFormatting(false);

        xmlWriter.writeStartDocument();

        xmlWriter.writeStartElement("rdf:RDF");
        xmlWriter.writeAttribute("xmlns:rdf", "http://www.w3.org/1999/02/22-rdf-syntax-ns#");
        xmlWriter.writeEndElement();

        xmlWriter.writeEndDocument();

        return xml;
    }

    QString ExportOdtPlugin::createMeta(ProjectModel const* model) const noexcept
    {
        QString xml;
        QXmlStreamWriter xmlWriter(&xml);
        xmlWriter.setCodec("UTF-8");
        xmlWriter.setAutoFormatting(false);

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

        xmlWriter.writeStartElement("dc:title");
        xmlWriter.writeCharacters(model->properties().m_name);
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

    QString ExportOdtPlugin::createSettings() const noexcept
    {
        return QString();
    }

    QString ExportOdtPlugin::createStyles(ProjectModel const* model) const noexcept
    {
        int headingDepth = maxHeadingDepth(model, model->projectRootIndex());

        QString xml;
        QXmlStreamWriter xmlWriter(&xml);
        xmlWriter.setCodec("UTF-8");
        xmlWriter.setAutoFormatting(false);

        xmlWriter.writeStartDocument();
        {
            xmlWriter.writeStartElement("office:document-styles");
            xmlWriter.writeAttribute("office:version", "1.2");
            xmlWriter.writeAttribute("xmlns:calcext",
                    "urn:org:documentfoundation:names:experimental:calc:xmlns:calcext:1.0");
            xmlWriter.writeAttribute("xmlns:chart", "urn:oasis:names:tc:opendocument:xmlns:chart:1.0");
            xmlWriter.writeAttribute("xmlns:dr3d", "urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0");
            xmlWriter.writeAttribute("xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0");
            xmlWriter.writeAttribute("xmlns:field", "urn:openoffice:names:experimental:ooo-ms-interop:xmlns:field:1.0");
            xmlWriter.writeAttribute("xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0");
            xmlWriter.writeAttribute("xmlns:form", "urn:oasis:names:tc:opendocument:xmlns:form:1.0");
            xmlWriter.writeAttribute("xmlns:loext",
                    "urn:org:documentfoundation:names:experimental:office:xmlns:loext:1.0");
            xmlWriter.writeAttribute("xmlns:meta", "urn:oasis:names:tc:opendocument:xmlns:meta:1.0");
            xmlWriter.writeAttribute("xmlns:number", "urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0");
            xmlWriter.writeAttribute("xmlns:of", "urn:oasis:names:tc:opendocument:xmlns:of:1.2");
            xmlWriter.writeAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
            xmlWriter.writeAttribute("xmlns:script", "urn:oasis:names:tc:opendocument:xmlns:script:1.0");
            xmlWriter.writeAttribute("xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
            xmlWriter.writeAttribute("xmlns:svg", "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0");
            xmlWriter.writeAttribute("xmlns:table", "urn:oasis:names:tc:opendocument:xmlns:table:1.0");
            xmlWriter.writeAttribute("xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0");

            {
                xmlWriter.writeStartElement("office:styles");

                // Default style
                {
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
                    xmlWriter.writeAttribute("fo:font-size", "12pt");
                    xmlWriter.writeAttribute("fo:language", lang::languageCode(model->properties().m_lang));
                    xmlWriter.writeAttribute("fo:country", lang::countryCode(model->properties().m_lang));
                    xmlWriter.writeAttribute("style:letter-kerning", "true");
                    xmlWriter.writeAttribute("fo:hyphenate", "true");
                    xmlWriter.writeAttribute("fo:hyphenation-remain-char-count", "2");
                    xmlWriter.writeAttribute("fo:hyphenation-push-char-count", "2");
                    xmlWriter.writeEndElement();

                    xmlWriter.writeEndElement();
                }

                // Heading style
                {
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
                }

                // Actual heading styles
                std::array<QString, 4> topMargins{"0.1665in", "0.139in", "0.0972in", "0.0835in"};
                std::array<QString, 4> fontSizes{"130%", "115%", "101%", "100%"};
                for (int d = 0; d < headingDepth; ++d) {
                    QString num = QString::number(d + 1);

                    xmlWriter.writeStartElement("style:style");
                    xmlWriter.writeAttribute("style:name", "Heading" + num);
                    xmlWriter.writeAttribute("style:display-name", "Heading " + num);
                    xmlWriter.writeAttribute("style:parent-style-name", "Heading");
                    xmlWriter.writeAttribute("style:family", "paragraph");
                    xmlWriter.writeAttribute("style:default-outline-level", num);
                    xmlWriter.writeAttribute("style:class", "text");

                    xmlWriter.writeStartElement("style:paragraph-properties");
                    xmlWriter.writeAttribute("fo:margin-top",
                            static_cast<size_t>(d) < topMargins.size() ? topMargins[d] : topMargins.back());
                    xmlWriter.writeEndElement();

                    xmlWriter.writeStartElement("style:text-properties");
                    xmlWriter.writeAttribute("fo:font-size",
                            static_cast<size_t>(d) < fontSizes.size() ? fontSizes[d] : fontSizes.back());
                    xmlWriter.writeEndElement();

                    xmlWriter.writeEndElement();
                }

                // Text body style
                {
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
                    xmlWriter.writeAttribute("fo:text-align", "justify");
                    xmlWriter.writeAttribute("style:auto-text-indent", "false");
                    xmlWriter.writeEndElement();

                    xmlWriter.writeEndElement();
                }

                // Title style
                {
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
                }

                // Subtitle style
                {
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
                }

                // Small caps style
                {
                    xmlWriter.writeStartElement("style:style");
                    xmlWriter.writeAttribute("style:name", "SmallCaps");
                    xmlWriter.writeAttribute("style:display-name", "Small Caps");
                    xmlWriter.writeAttribute("style:parent-style-name", "TextBody");
                    xmlWriter.writeAttribute("style:family", "text");

                    xmlWriter.writeStartElement("style:text-properties");
                    xmlWriter.writeAttribute("fo:font-variant", "small-caps");
                    xmlWriter.writeEndElement();

                    xmlWriter.writeEndElement();
                }

                // Bold style
                {
                    xmlWriter.writeStartElement("style:style");
                    xmlWriter.writeAttribute("style:name", "Bold");
                    xmlWriter.writeAttribute("style:parent-style-name", "TextBody");
                    xmlWriter.writeAttribute("style:family", "text");

                    xmlWriter.writeStartElement("style:text-properties");
                    xmlWriter.writeAttribute("fo:font-weight", "bold");
                    xmlWriter.writeEndElement();

                    xmlWriter.writeEndElement();
                }

                // Italic style
                {
                    xmlWriter.writeStartElement("style:style");
                    xmlWriter.writeAttribute("style:name", "Italic");
                    xmlWriter.writeAttribute("style:parent-style-name", "TextBody");
                    xmlWriter.writeAttribute("style:family", "text");

                    xmlWriter.writeStartElement("style:text-properties");
                    xmlWriter.writeAttribute("fo:font-style", "italic");
                    xmlWriter.writeEndElement();

                    xmlWriter.writeEndElement();
                }

                // Underline style
                {
                    xmlWriter.writeStartElement("style:style");
                    xmlWriter.writeAttribute("style:name", "Underline");
                    xmlWriter.writeAttribute("style:parent-style-name", "TextBody");
                    xmlWriter.writeAttribute("style:family", "text");

                    xmlWriter.writeStartElement("style:text-properties");
                    xmlWriter.writeAttribute("text-underline-style", "solid");
                    xmlWriter.writeAttribute("text-underline-width", "auto");
                    xmlWriter.writeAttribute("text-underline-color", "font-color");
                    xmlWriter.writeEndElement();

                    xmlWriter.writeEndElement();
                }

                // Strikethrough style
                {
                    xmlWriter.writeStartElement("style:style");
                    xmlWriter.writeAttribute("style:name", "Strikethrough");
                    xmlWriter.writeAttribute("style:parent-style-name", "TextBody");
                    xmlWriter.writeAttribute("style:family", "text");

                    xmlWriter.writeStartElement("style:text-properties");
                    xmlWriter.writeAttribute("style:text-line-through-style", "solid");
                    xmlWriter.writeAttribute("style:text-line-through-type", "single");
                    xmlWriter.writeEndElement();

                    xmlWriter.writeEndElement();
                }

                // Overline style
                {
                    xmlWriter.writeStartElement("style:style");
                    xmlWriter.writeAttribute("style:name", "Overline");
                    xmlWriter.writeAttribute("style:parent-style-name", "TextBody");
                    xmlWriter.writeAttribute("style:family", "text");

                    xmlWriter.writeStartElement("style:text-properties");
                    xmlWriter.writeAttribute("style:text-overline-style", "solid");
                    xmlWriter.writeAttribute("style:text-overline-width", "auto");
                    xmlWriter.writeAttribute("style:text-overline-color", "font-color");
                    xmlWriter.writeEndElement();

                    xmlWriter.writeEndElement();
                }

                xmlWriter.writeEndElement();
            }

            // Page layout
            {
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
            }

            xmlWriter.writeStartElement("office:master-styles");
            xmlWriter.writeStartElement("style::master-page");
            xmlWriter.writeAttribute("style:name", "Standard");
            xmlWriter.writeAttribute("style:page-layout-name", "PageLayout");
            xmlWriter.writeEndElement();
            xmlWriter.writeEndElement();

            xmlWriter.writeEndElement();
        }

        xmlWriter.writeEndDocument();

        return xml;
    }

    QString ExportOdtPlugin::createContent(ProjectModel const* model) const noexcept
    {
        QString xml;
        QXmlStreamWriter xmlWriter(&xml);
        xmlWriter.setCodec("UTF-8");
        xmlWriter.setAutoFormatting(false);

        xmlWriter.writeStartDocument();

        xmlWriter.writeStartElement("office:document-content");
        xmlWriter.writeAttribute("xmlns:calcext",
                "urn:org:documentfoundation:names:experimental:calc:xmlns:calcext:1.0");
        xmlWriter.writeAttribute("xmlns:chart", "urn:oasis:names:tc:opendocument:xmlns:chart:1.0");
        xmlWriter.writeAttribute("xmlns:dr3d", "urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0");
        xmlWriter.writeAttribute("xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0");
        xmlWriter.writeAttribute("xmlns:field", "urn:openoffice:names:experimental:ooo-ms-interop:xmlns:field:1.0");
        xmlWriter.writeAttribute("xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0");
        xmlWriter.writeAttribute("xmlns:form", "urn:oasis:names:tc:opendocument:xmlns:form:1.0");
        xmlWriter.writeAttribute("xmlns:loext", "urn:org:documentfoundation:names:experimental:office:xmlns:loext:1.0");
        xmlWriter.writeAttribute("xmlns:meta", "urn:oasis:names:tc:opendocument:xmlns:meta:1.0");
        xmlWriter.writeAttribute("xmlns:number", "urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0");
        xmlWriter.writeAttribute("xmlns:of", "urn:oasis:names:tc:opendocument:xmlns:of:1.2");
        xmlWriter.writeAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
        xmlWriter.writeAttribute("xmlns:script", "urn:oasis:names:tc:opendocument:xmlns:script:1.0");
        xmlWriter.writeAttribute("xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
        xmlWriter.writeAttribute("xmlns:svg", "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0");
        xmlWriter.writeAttribute("xmlns:table", "urn:oasis:names:tc:opendocument:xmlns:table:1.0");
        xmlWriter.writeAttribute("xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0");
        xmlWriter.writeAttribute("office:version", "1.2");
        xmlWriter.writeAttribute("office:mimetype", "application/vnd.oasis.opendocument.text");

        xmlWriter.writeStartElement("office:automatic-styles");

        // paragraph without text indentation
        xmlWriter.writeStartElement("style:style");
        xmlWriter.writeAttribute("style:name", "par_no_indent");
        xmlWriter.writeAttribute("style:family", "paragraph");
        xmlWriter.writeAttribute("style:parent-style-name", "TextBody");
        xmlWriter.writeStartElement("style:paragraph-properties");
        xmlWriter.writeAttribute("fo:text-indent", "0in");
        xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();

        // paragraph without text indentation but with long skip
        xmlWriter.writeStartElement("style:style");
        xmlWriter.writeAttribute("style:name", "par_long_skip");
        xmlWriter.writeAttribute("style:family", "paragraph");
        xmlWriter.writeAttribute("style:parent-style-name", "TextBody");
        xmlWriter.writeStartElement("style:paragraph-properties");
        xmlWriter.writeAttribute("fo:text-indent", "0in");
        xmlWriter.writeAttribute("fo:margin-top", "0.4in");
        xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();

        // Title
        xmlWriter.writeStartElement("style:style");
        xmlWriter.writeAttribute("style:name", "par_title");
        xmlWriter.writeAttribute("style:family", "paragraph");
        xmlWriter.writeAttribute("style:parent-style-name", "Title");
        xmlWriter.writeStartElement("style:paragraph-properties");
        xmlWriter.writeAttribute("fo:margin-top", "4in");
        xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();

        // Author
        xmlWriter.writeStartElement("style:style");
        xmlWriter.writeAttribute("style:name", "par_author");
        xmlWriter.writeAttribute("style:family", "paragraph");
        xmlWriter.writeAttribute("style:parent-style-name", "Subtitle");
        xmlWriter.writeStartElement("style:paragraph-properties");
        xmlWriter.writeAttribute("fo:break-after", "page");
        xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();

        // content
        xmlWriter.writeStartElement("office:body");
        xmlWriter.writeStartElement("office:text");
        xmlWriter.writeStartElement("text:p");
        xmlWriter.writeAttribute("text:style-name", "par_title");
        xmlWriter.writeCharacters(model->properties().m_name);
        xmlWriter.writeEndElement();
        xmlWriter.writeStartElement("text:p");
        xmlWriter.writeAttribute("text:style-name", "par_author");
        xmlWriter.writeCharacters(model->properties().m_author);
        xmlWriter.writeEndElement();

        // chapters
        writeChapterXml(xmlWriter, model, model->projectRootIndex());

        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();

        xmlWriter.writeEndDocument();

        return xml;
    }
}
