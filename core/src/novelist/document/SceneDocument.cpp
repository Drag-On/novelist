/**********************************************************
 * @file   SceneDocument.cpp
 * @author jan
 * @date   8/6/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <QTextStream>
#include <QTextBlockFormat>
#include <QTextBlock>
#include <QDebug>
#include <gsl/gsl_assert>
#include <gsl/gsl_util>
#include <QtGui/QtGui>
#include "document/SceneDocument.h"

namespace novelist {

    SceneDocument::SceneDocument(Language lang, QObject* parent)
            :SceneDocument("", lang, parent)
    {
    }

    SceneDocument::SceneDocument(QString text, Language lang, QObject* parent)
            :QTextDocument(text, parent),
             m_insightMgr(static_cast<QTextDocument*>(this)),
             m_lang(lang)
    {
        m_insightMgr.setDocument(this);
    }

    bool SceneDocument::read(QFile& file)
    {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString xml = file.readAll();
        file.close();

        return read(xml);
    }

    bool SceneDocument::read(QString const& xml)
    {
        clear();

        QXmlStreamReader xmlReader(xml);
        if (xmlReader.readNextStartElement()) {
            if (xmlReader.name() == "scene" && xmlReader.attributes().value("version") == "1.0")
                if (readInternal(xmlReader)) {
                    setModified(false);
                    return true;
                }

        }
        if (xmlReader.hasError())
            qWarning() << "Error while reading scene file." << xmlReader.errorString() << "At line"
                       << xmlReader.lineNumber() << ", column" << xmlReader.columnNumber() << ", character offset"
                       << xmlReader.characterOffset();

        return false;
    }

    bool SceneDocument::write(QFile& file) const
    {
        QString xml;
        if (!write(xml))
            return false;

        if (!file.open(QIODevice::WriteOnly))
            return false;

        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        stream << xml;

        return true;
    }

    bool SceneDocument::write(QString& xml) const
    {
        QXmlStreamWriter xmlWriter(&xml);
        xmlWriter.setCodec("UTF-8");
        xmlWriter.setAutoFormatting(true);

        xmlWriter.writeStartDocument();
        xmlWriter.writeDTD("<!DOCTYPE scene>");
        xmlWriter.writeStartElement("scene");
        xmlWriter.writeAttribute("version", "1.0");

        xmlWriter.writeStartElement("content");
        for (auto b = begin(); b != end(); b = b.next()) {
            if (b.isValid()) {
                if (!writeBlock(xmlWriter, b))
                    return false;
            }
        }
        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();

        xmlWriter.writeEndDocument();

        return true;
    }

    Language SceneDocument::language() const noexcept
    {
        return m_lang;
    }

    void SceneDocument::setLanguage(Language lang) noexcept
    {
        m_lang = lang;
    }

    bool SceneDocument::operator==(SceneDocument const& other) const
    {
        if (blockCount() != other.blockCount())
            return false;
        auto thisBlock = begin();
        auto otherBlock = other.begin();
        for (int i = 0; i < blockCount(); ++i) {
            if (thisBlock.text() != otherBlock.text())
                return false;
            thisBlock = thisBlock.next();
            otherBlock = otherBlock.next();
        }

        return true;
    }

    bool SceneDocument::operator!=(SceneDocument const& other) const
    {
        return !(*this == other);
    }

    SceneDocumentInsightManager& SceneDocument::insightManager()
    {
        return m_insightMgr;
    }

    bool SceneDocument::readInternal(QXmlStreamReader& xml)
    {
        Expects(xml.isStartElement() && xml.name() == "scene");

        // Disable undo/redo for the read process, enable again afterwards
        setUndoRedoEnabled(false);
        auto cleanup = gsl::finally([this]() { setUndoRedoEnabled(true); });

        QTextCursor cursor(this);
        cursor.movePosition(QTextCursor::End);

        while (xml.readNextStartElement()) {
            if (xml.name() == "content") {
                while (xml.readNextStartElement()) {
                    if (!readBlock(xml, cursor))
                        return false;
                }
            }
            else
                xml.skipCurrentElement();
        }

        // Remove first block (which was there by default)
        cursor.movePosition(QTextCursor::Start);
        cursor.select(QTextCursor::BlockUnderCursor);
        cursor.removeSelectedText();
        cursor.deleteChar(); // Note: previous operation should be sufficient, yet on the first block this is needed.
        return true;
    }

    bool SceneDocument::readBlock(QXmlStreamReader& xml, QTextCursor& cursor)
    {
        Expects(xml.isStartElement() && xml.name() == "block");

        QTextBlockFormat format = readBlockFormatAttr(xml);
        cursor.insertBlock(format);

        return readText(xml, cursor);
    }

    bool SceneDocument::readText(QXmlStreamReader& xml, QTextCursor& cursor)
    {
        while (xml.readNextStartElement() && xml.name() == "text") {
            QTextCharFormat format = readCharFormatAttr(xml);
            cursor.insertText(xml.readElementText(), format);
        }

        return true;
    }

    QTextBlockFormat SceneDocument::readBlockFormatAttr(QXmlStreamReader& xml) const
    {
        QTextBlockFormat format;
//        if (xml.attributes().hasAttribute("align"))
//            format.setAlignment(static_cast<Qt::Alignment>(xml.attributes().value("align").toInt()));
//        if (xml.attributes().hasAttribute("botMargin"))
//            format.setBottomMargin(xml.attributes().value("botMargin").toFloat());
//        if (xml.attributes().hasAttribute("topMargin"))
//            format.setTopMargin(xml.attributes().value("topMargin").toFloat());
//        if (xml.attributes().hasAttribute("leftMargin"))
//            format.setLeftMargin(xml.attributes().value("leftMargin").toFloat());
//        if (xml.attributes().hasAttribute("rightMargin"))
//            format.setRightMargin(xml.attributes().value("rightMargin").toFloat());
//        if (xml.attributes().hasAttribute("indent"))
//            format.setIndent(xml.attributes().value("indent").toInt());
//        if (xml.attributes().hasAttribute("lineHeight") && xml.attributes().hasAttribute("lineHeightType"))
//            format.setLineHeight(xml.attributes().value("lineHeight").toFloat(),
//                    xml.attributes().value("lineHeightType").toInt());
        if (xml.attributes().hasAttribute("textIndent"))
            format.setTextIndent(xml.attributes().value("textIndent").toFloat());

        return format;
    }

    QTextCharFormat SceneDocument::readCharFormatAttr(QXmlStreamReader& xml) const
    {
        QTextCharFormat format;
        if (xml.attributes().hasAttribute("capitalization"))
            format.setFontCapitalization(
                    static_cast<QFont::Capitalization>(xml.attributes().value("capitalization").toInt()));
        if (xml.attributes().hasAttribute("italic"))
            format.setFontItalic(xml.attributes().value("italic").toInt() != 0);
        if (xml.attributes().hasAttribute("overline"))
            format.setFontOverline(xml.attributes().value("overline").toInt() != 0);
        if (xml.attributes().hasAttribute("strikeout"))
            format.setFontStrikeOut(xml.attributes().value("strikeout").toInt() != 0);
        if (xml.attributes().hasAttribute("underline"))
            format.setFontUnderline(xml.attributes().value("underline").toInt() != 0);
        if (xml.attributes().hasAttribute("weight"))
            format.setFontWeight(xml.attributes().value("weight").toInt());
//        if (xml.attributes().hasAttribute("font"))
//            format.setFontFamily(xml.attributes().value("font").toString());
//        if (xml.attributes().hasAttribute("size"))
//            format.setFontPointSize(xml.attributes().value("size").toFloat());
//        if (xml.attributes().hasAttribute("style"))
//            format.setFontStyleHint(static_cast<QFont::StyleHint>(xml.attributes().value("style").toInt()),
//                    format.fontStyleStrategy());

        return format;
    }

    bool SceneDocument::writeBlock(QXmlStreamWriter& xml, QTextBlock const& block) const
    {
        xml.writeStartElement("block");
        writeBlockFormatAttr(xml, block.blockFormat());

        for (auto iter = block.begin(); iter != block.end(); ++iter) {
            QTextFragment currentFragment = iter.fragment();
            if (currentFragment.isValid()) {
                if (!writeFragment(xml, currentFragment))
                    return false;
            }
        }

        xml.writeEndElement();

        return true;
    }

    bool SceneDocument::writeFragment(QXmlStreamWriter& xml, QTextFragment const& fragment) const
    {
        xml.writeStartElement("text");
        writeCharFormatAttr(xml, fragment.charFormat());
        xml.writeCharacters(fragment.text());
        xml.writeEndElement();

        return true;
    }

    bool SceneDocument::writeBlockFormatAttr(QXmlStreamWriter& xml, QTextBlockFormat const& format) const
    {
//        xml.writeAttribute("align", QString::number(static_cast<int>(format.alignment())));
//        xml.writeAttribute("botMargin", QString::number(format.bottomMargin()));
//        xml.writeAttribute("topMargin", QString::number(format.topMargin()));
//        xml.writeAttribute("rightMargin", QString::number(format.rightMargin()));
//        xml.writeAttribute("leftMargin", QString::number(format.leftMargin()));
//        xml.writeAttribute("indent", QString::number(format.indent()));
        xml.writeAttribute("textIndent", QString::number(format.textIndent()));
//        xml.writeAttribute("lineHeight", QString::number(format.lineHeight()));
//        xml.writeAttribute("lineHeightType", QString::number(format.lineHeightType()));
        return true;
    }

    bool SceneDocument::writeCharFormatAttr(QXmlStreamWriter& xml, QTextCharFormat const& format) const
    {
        xml.writeAttribute("capitalization", QString::number(format.font().capitalization()));
        xml.writeAttribute("italic", QString::number(format.font().italic()));
        xml.writeAttribute("overline", QString::number(format.font().overline()));
        xml.writeAttribute("underline", QString::number(format.font().underline()));
        xml.writeAttribute("strikeout", QString::number(format.font().strikeOut()));
        xml.writeAttribute("weight", QString::number(format.font().weight()));
//        xml.writeAttribute("font", format.font().family());
//        xml.writeAttribute("style", QString::number(format.font().styleHint()));
//        xml.writeAttribute("size", QString::number(format.font().pointSizeF()));
        return true;
    }
}