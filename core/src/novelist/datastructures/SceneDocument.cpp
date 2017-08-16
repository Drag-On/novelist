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
#include <QTextCursor>
#include <gsl/gsl_assert>
#include "datastructures/SceneDocument.h"

namespace novelist
{
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
        if(!write(xml))
            return false;

        if (!file.open(QIODevice::WriteOnly))
            return false;

        QTextStream stream(&file);
        stream << xml;

        return true;
    }

    bool SceneDocument::write(QString& xml) const
    {
        QXmlStreamWriter xmlWriter(&xml);
        xmlWriter.setAutoFormatting(true);

        xmlWriter.writeStartDocument();
        xmlWriter.writeDTD("<!DOCTYPE scene>");
        xmlWriter.writeStartElement("scene");
        xmlWriter.writeAttribute("version", "1.0");

        xmlWriter.writeStartElement("content");
        for(auto b = begin(); b != end(); b = b.next())
        {
            if(b.isValid())
            {
                if(!writeBlock(xmlWriter, b))
                    return false;
            }
        }
        xmlWriter.writeEndElement();

        xmlWriter.writeEndElement();

        xmlWriter.writeEndDocument();

        return true;
    }

    bool SceneDocument::readInternal(QXmlStreamReader& xml)
    {
        Expects(xml.isStartElement() && xml.name() == "scene");

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

        QTextBlockFormat format;
        if (xml.attributes().hasAttribute("align"))
            format.setAlignment(static_cast<Qt::Alignment>(xml.attributes().value("align").toInt()));
        if (xml.attributes().hasAttribute("botMargin"))
            format.setBottomMargin(xml.attributes().value("botMargin").toFloat());
        if (xml.attributes().hasAttribute("topMargin"))
            format.setTopMargin(xml.attributes().value("topMargin").toFloat());
        if (xml.attributes().hasAttribute("leftMargin"))
            format.setLeftMargin(xml.attributes().value("leftMargin").toFloat());
        if (xml.attributes().hasAttribute("rightMargin"))
            format.setRightMargin(xml.attributes().value("rightMargin").toFloat());
        if (xml.attributes().hasAttribute("indent"))
            format.setIndent(xml.attributes().value("indent").toInt());
        if (xml.attributes().hasAttribute("lineHeight") && xml.attributes().hasAttribute("lineHeightType"))
            format.setLineHeight(xml.attributes().value("lineHeight").toFloat(), xml.attributes().value("lineHeightType").toInt());
        if (xml.attributes().hasAttribute("textIndent"))
            format.setTextIndent(xml.attributes().value("textIndent").toFloat());

        cursor.insertBlock(format);

        return readText(xml, cursor);
    }

    bool SceneDocument::readText(QXmlStreamReader& xml, QTextCursor& cursor)
    {
        while (xml.readNextStartElement() && xml.name() == "text") {

            QTextCharFormat format;
            if (xml.attributes().hasAttribute("capitalization"))
                format.setFontCapitalization(static_cast<QFont::Capitalization>(xml.attributes().value("capitalization").toInt()));
            if (xml.attributes().hasAttribute("font"))
                format.setFontFamily(xml.attributes().value("font").toString());
            if (xml.attributes().hasAttribute("italic"))
                format.setFontItalic(xml.attributes().value("italic").toInt() != 0);
            if (xml.attributes().hasAttribute("overline"))
                format.setFontOverline(xml.attributes().value("overline").toInt() != 0);
            if (xml.attributes().hasAttribute("size"))
                format.setFontPointSize(xml.attributes().value("size").toFloat());
            if (xml.attributes().hasAttribute("strikeout"))
                format.setFontStrikeOut(xml.attributes().value("strikeout").toInt() != 0);
            if (xml.attributes().hasAttribute("style"))
                format.setFontStyleHint(static_cast<QFont::StyleHint>(xml.attributes().value("style").toInt()), format.fontStyleStrategy());
            if (xml.attributes().hasAttribute("underline"))
                format.setFontUnderline(xml.attributes().value("underline").toInt() != 0);
            if (xml.attributes().hasAttribute("weight"))
                format.setFontWeight(xml.attributes().value("weight").toInt());

            cursor.insertText(xml.readElementText(), format);
        }

        return true;
    }

    bool SceneDocument::writeBlock(QXmlStreamWriter& xml, QTextBlock const& block) const
    {
        xml.writeStartElement("block");
        xml.writeAttribute("align", QString::number(static_cast<int>(block.blockFormat().alignment())));
        xml.writeAttribute("botMargin", QString::number(block.blockFormat().bottomMargin()));
        xml.writeAttribute("topMargin", QString::number(block.blockFormat().topMargin()));
        xml.writeAttribute("rightMargin", QString::number(block.blockFormat().rightMargin()));
        xml.writeAttribute("leftMargin", QString::number(block.blockFormat().leftMargin()));
        xml.writeAttribute("indent", QString::number(block.blockFormat().indent()));
        xml.writeAttribute("textIndent", QString::number(block.blockFormat().textIndent()));
        xml.writeAttribute("lineHeight", QString::number(block.blockFormat().lineHeight()));
        xml.writeAttribute("lineHeightType", QString::number(block.blockFormat().lineHeightType()));

        for(auto iter = block.begin(); iter != block.end(); ++iter)
        {
            QTextFragment currentFragment = iter.fragment();
            if (currentFragment.isValid())
            {
                if(!writeFragment(xml, currentFragment))
                    return false;
            }
        }

        xml.writeEndElement();

        return true;
    }

    bool SceneDocument::writeFragment(QXmlStreamWriter& xml, QTextFragment const& fragment) const
    {
        QFont const& font = fragment.charFormat().font();
        xml.writeStartElement("text");
        xml.writeAttribute("capitalization", QString::number(font.capitalization()));
        xml.writeAttribute("italic", QString::number(font.italic()));
        xml.writeAttribute("overline", QString::number(font.overline()));
        xml.writeAttribute("underline", QString::number(font.underline()));
        xml.writeAttribute("strikeout", QString::number(font.strikeOut()));
        xml.writeAttribute("weight", QString::number(font.weight()));
        xml.writeAttribute("font", font.family());
        xml.writeAttribute("style", QString::number(font.styleHint()));
        xml.writeAttribute("size", QString::number(font.pointSizeF()));

        xml.writeCharacters(fragment.text());

        xml.writeEndElement();

        return true;
    }
}