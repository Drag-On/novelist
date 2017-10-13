/**********************************************************
 * @file   TextMarkerTest.cpp
 * @author jan
 * @date   10/11/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <catch.hpp>
#include <QtGui/QTextCursor>
#include <QApplication>
#include <macros.h>
#include "document/TextMarker.h"

using namespace novelist;

TEST_CASE("TextMarker construction", "[TextMarker][Document]")
{
    QTextDocument doc;
    REQUIRE_NOTHROW(([&doc]() { TextMarker marker{&doc, 0, 0, QTextCharFormat()}; }()));

    REQUIRE_THROWS(([&doc]() { TextMarker marker{&doc, -1, 0, QTextCharFormat()}; }()));
    REQUIRE_THROWS(([&doc]() { TextMarker marker{&doc, 10, 12, QTextCharFormat()}; }()));
    REQUIRE_THROWS(([&doc]() { TextMarker marker{&doc, -100, -1000, QTextCharFormat()}; }()));
    REQUIRE_THROWS(([&doc]() { TextMarker marker{&doc, 0, -12, QTextCharFormat()}; }()));

    doc.setPlainText("Some text");

    REQUIRE_NOTHROW(([&doc]() { TextMarker marker{&doc, 0, 0, QTextCharFormat()}; }()));
    REQUIRE_NOTHROW(([&doc]() { TextMarker marker{&doc, 0, 4, QTextCharFormat()}; }()));
    REQUIRE_NOTHROW(([&doc]() { TextMarker marker{&doc, 2, 4, QTextCharFormat()}; }()));
    REQUIRE_NOTHROW(([&doc]() { TextMarker marker{&doc, 0, 9, QTextCharFormat()}; }()));

    REQUIRE_THROWS(([&doc]() { TextMarker marker{&doc, 0, 10, QTextCharFormat()}; }()));
}

TEST_CASE("TextMarker movement", "[TextMarker][Document]")
{
    QTextDocument doc;
    doc.setPlainText("This is a text. It isn't a long one, but it's something to work with.");

    // Due to a bug in Qt, the QTextDocument::contentsChange signal doesn't fire. Calling QTextDocument::documentLayout
    // before changes is a workaround, see https://bugreports.qt.io/browse/QTBUG-43695?focusedCommentId=269655&page=com.atlassian.jira.plugin.system.issuetabpanels:comment-tabpanel#comment-269655
    doc.documentLayout();

    auto checkMarker = [&doc](TextMarker const& m)
    {
        QTextCursor cursor = m.toCursor();
        INFO("Marked range is " << m << " and text is \"" << doc.toPlainText().toStdString() << "\"");
        REQUIRE(cursor.selectedText().toStdString() == "text");
    };

    TextMarker marker{&doc, 10, 14, QTextCharFormat()};
    checkMarker(marker);

    DATA_SECTION("Stability on insertion",
            TESTFUN([&](int pos, QString s) {
                QTextCursor cursor{&doc};
                cursor.setPosition(pos);
                cursor.insertText(s);
                checkMarker(marker);
            }),
            NAMED_ROW("Insert after marker", 16, "Really. ")
            NAMED_ROW("Insert at end", doc.toPlainText().length(), " Woopsie.")
            NAMED_ROW("Insert before marker", 10, "nice ")
            NAMED_ROW("Insert at beginning", 0, "Whoa. ")
            NAMED_ROW("Insert new block before marker", 0, "This is a new block.\n")
            NAMED_ROW("Insert new block after marker", doc.toPlainText().length(), "\nThis is a new block.")
    );

    DATA_SECTION("Stability on deletion",
            TESTFUN([&](int pos, int length) {
                QTextCursor cursor{&doc};
                cursor.setPosition(pos);
                cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, length);
                cursor.removeSelectedText();
                checkMarker(marker);
            }),
            NAMED_ROW("Remove after marker", 35, 34)
            NAMED_ROW("Remove before marker", 7, 2)
    );

    DATA_SECTION("Stability on replace",
            TESTFUN([&](int pos, int length, QString replacement) {
                QTextCursor cursor{&doc};
                cursor.setPosition(pos);
                cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, length);
                cursor.insertText(replacement);
                checkMarker(marker);
            }),
            NAMED_ROW("Replace after marker", 16, 19, "It's great")
            NAMED_ROW("Replace before marker", 5, 2, "used to be")
    )

    SECTION("Completely erase")
    {
        bool collapsedCalled = false;
        QObject::connect(&marker, &TextMarker::collapsed, [&collapsedCalled](TextMarker&) {collapsedCalled=true;} );
        QTextCursor cursor = marker.toCursor();
        cursor.removeSelectedText();
        REQUIRE(collapsedCalled);
    }
}
