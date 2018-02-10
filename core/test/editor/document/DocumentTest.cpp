/**********************************************************
 * @file   DocumentTest.cpp
 * @author jan
 * @date   1/27/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include "test/catch_string_conversions.h"
#include <catch.hpp>
#include <editor/document/Document.h>
#include <editor/document/TextCursor.h>

using namespace novelist::editor;

TEST_CASE("Document", "[editor][document]")
{
    // Prepare a format manager
    TextFormatData defaultFormat;
    defaultFormat.m_name = "Default";

    TextFormatData dreamFormat;
    dreamFormat.m_name = "Dream";
    dreamFormat.m_characterFormat.m_italic = true;

    TextFormatData lyricFormat;
    lyricFormat.m_name = "Lyric";
    lyricFormat.m_indentation.m_autoTextIndent = false;
    lyricFormat.m_margin.m_top = 5;
    lyricFormat.m_margin.m_bottom = 5;

    TextFormatData settingFormat;
    settingFormat.m_name = "Setting";
    settingFormat.m_alignment = Alignment::Center;
    settingFormat.m_indentation.m_autoTextIndent = false;
    settingFormat.m_margin.m_bottom = 10;

    TextFormatManager formatMgr;
    auto defaultFormatId = formatMgr.push_back(defaultFormat);
    auto dreamFormatId = formatMgr.push_back(dreamFormat);
    auto lyricFormatId = formatMgr.push_back(lyricFormat);
    auto settingFormatId = formatMgr.push_back(settingFormat);

    Document doc(&formatMgr, "Test Document", getProjectLanguage(Language::English, Country::UnitedStates));
    TextCursor cursor(&doc);
    SECTION("Cursor state on empty doc") {
        REQUIRE_NOTHROW(cursor.paragraphFormat());
        REQUIRE_NOTHROW(cursor.characterFormat());
    }
    SECTION("Write to empty") {
        cursor.insertText("Hello, World!");
        REQUIRE(doc.toRawText() == "Hello, World!");
    }
    SECTION("Character formats") {
        cursor.insertText("Hello, World!");
        cursor.select(7, 12);
        REQUIRE(cursor.selectedText() == "World");
        cursor.setCharacterFormat(dreamFormatId);
        REQUIRE(cursor.characterFormat() == defaultFormatId);
        cursor.setPosition(8);
        REQUIRE(cursor.characterFormat() == dreamFormatId);
        cursor.setPosition(12);
        REQUIRE(cursor.characterFormat() == dreamFormatId);
        cursor.setPosition(13);
        REQUIRE(cursor.characterFormat() == defaultFormatId);
        cursor.setPosition(9);
        cursor.insertText("ooo");
        cursor.select(7, 15);
        REQUIRE(cursor.selectedText() == "Woooorld");
        cursor.setPosition(10);
        REQUIRE(cursor.characterFormat() == dreamFormatId);
    }
    SECTION("Paragraph formats") {
        cursor.insertText("Hello, World!");
        cursor.breakParagraph();
        REQUIRE(cursor.characterFormat() == defaultFormatId);
        REQUIRE(cursor.paragraphFormat() == defaultFormatId);
        cursor.insertText("Lorem ipsum dolor sit amet.");
        cursor.select(20, 25);
        cursor.setCharacterFormat(dreamFormatId);
        cursor.setPosition(22);
        cursor.breakParagraph();
        REQUIRE(cursor.atParagraphStart());
        REQUIRE(cursor.characterFormat() == dreamFormatId);
        REQUIRE(cursor.paragraphFormat() == defaultFormatId);
        cursor.setPosition(22);
        REQUIRE(cursor.atParagraphEnd());
        REQUIRE(cursor.characterFormat() == dreamFormatId);
        REQUIRE(cursor.paragraphFormat() == defaultFormatId);
        cursor.breakParagraph();
        REQUIRE(cursor.atParagraphStart());
        REQUIRE(cursor.characterFormat() == dreamFormatId);
        REQUIRE(cursor.paragraphFormat() == defaultFormatId);
    }
    SECTION("Undo / Redo") {
        cursor.insertText("Hello, World!");
        cursor.insertText(" Another horrific morning.");
        REQUIRE(doc.toRawText() == "Hello, World! Another horrific morning.");
        REQUIRE(doc.undoStack().canUndo());
        REQUIRE(!doc.undoStack().canRedo());
        doc.undoStack().undo();
        REQUIRE(doc.toRawText() == "Hello, World!");
        REQUIRE(doc.undoStack().canUndo());
        REQUIRE(doc.undoStack().canRedo());
        doc.undoStack().undo();
        REQUIRE(doc.toRawText() == "");
        REQUIRE(!doc.undoStack().canUndo());
        REQUIRE(doc.undoStack().canRedo());
        doc.undoStack().redo();
        REQUIRE(doc.toRawText() == "Hello, World!");

        cursor.insertText(" こんにちは世界！");
        REQUIRE(doc.toRawText() == "Hello, World! こんにちは世界！");
        REQUIRE(doc.undoStack().canUndo());
        REQUIRE(!doc.undoStack().canRedo());
        doc.undoStack().undo();
        REQUIRE(doc.toRawText() == "Hello, World!");
        REQUIRE(doc.undoStack().canUndo());
        REQUIRE(doc.undoStack().canRedo());
        doc.undoStack().redo();
        REQUIRE(doc.toRawText() == "Hello, World! こんにちは世界！");
    }
    SECTION("Paragraph iteration") {
        QString texts[] = {
                "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.",
                "Short paragraph.",
                "",
                "Slightly longer paragraph with 45 characters.",
        };

        cursor.insertText(texts[0]);
        cursor.breakParagraph();
        cursor.insertText(texts[1]);
        cursor.breakParagraph();
        cursor.breakParagraph();
        cursor.insertText(texts[3]);

        int i = 0;
        int lines = 0;
        for (auto const& p : doc)
        {
            REQUIRE(p.text() == texts[i]);
            REQUIRE(p.length() == texts[i].length() + 1); // +1 for newline
            REQUIRE(p.firstLineNo() == lines);
            ++i;
            lines += p.lineCount();
        }
        REQUIRE(i == sizeof(texts) / sizeof(*texts));
    }
}