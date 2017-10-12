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
#include <QDebug>
#include <QApplication>
#include <macros.h>
#include "document/TextMarker.h"

using namespace novelist;

TEST_CASE("TextMarker construction", "[TextMarker][Document]")
{
    QTextDocument doc;
    TextMarker {&doc, 0};
    TextMarker {&doc, 1}; // There is a newline character, therefore the length of doc is 1.
    REQUIRE_THROWS(([&doc]() { TextMarker marker1{&doc, -1}; }()));
    REQUIRE_THROWS(([&doc]() { TextMarker marker1{&doc, 10}; }()));

    doc.setPlainText("Some text");

    TextMarker {&doc, 0};
    TextMarker {&doc, 10};

    REQUIRE_THROWS(([&doc]() { TextMarker marker1{&doc, -1}; }()));
    REQUIRE_THROWS(([&doc]() { TextMarker marker1{&doc, 11}; }()));
}

TEST_CASE("TextMarker movement", "[TextMarker][Document]")
{
    int argc = 1;
    char str[] = "Foo";
    char* argv[] = { str };
    QApplication app(argc, argv); // QApplication requires at least one argument

    QTextDocument doc;
    doc.setPlainText("This is a text. It isn't a long one, but it's something to work with.");

    auto checkMarker = [&doc](TextMarker const& m)
    {
        QTextCursor cursor{&doc};
        cursor.setPosition(m.position());
        cursor.movePosition(QTextCursor::PreviousWord, QTextCursor::KeepAnchor);
        INFO("Position is " << m.position() << " and text is \"" << doc.toPlainText().toStdString() << "\"");
        REQUIRE(cursor.selectedText().toStdString() == "text");
    };

    // Due to a bug in Qt, the QTextDocument::contentsChange signal doesn't fire. Calling QTextDocument::documentLayout
    // before changes is a workaround, see https://bugreports.qt.io/browse/QTBUG-43695?focusedCommentId=269655&page=com.atlassian.jira.plugin.system.issuetabpanels:comment-tabpanel#comment-269655
    doc.documentLayout();

    TextMarker marker{&doc, 14};
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

    SECTION("Remove on marker")
    {
        int pos = marker.position();
        QTextCursor cursor{&doc};
        cursor.setPosition(pos);
        cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 1);
        cursor.removeSelectedText();
        REQUIRE(marker.position() == pos - 1);
    }

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

    DATA_SECTION("Replace over marker",
        TESTFUN([&](TextMarkerAttachment attachment) {
            marker.setAttachment(attachment);
            int pos = marker.position();
            QTextCursor cursor{&doc};
            cursor.setPosition(pos-1);
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 2);
            cursor.insertText("foo");
            int mod = attachment == TextMarkerAttachment::AttachLeft? -1 : 2;
            REQUIRE(marker.position() == pos + mod);
        }),
        NAMED_ROW("Left attachment", TextMarkerAttachment::AttachLeft)
        NAMED_ROW("Right attachment", TextMarkerAttachment::AttachRight)
    )
}
