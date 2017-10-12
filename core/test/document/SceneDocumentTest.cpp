/**********************************************************
 * @file   SceneDocumentTest.cpp
 * @author jan
 * @date   8/6/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <QDebug>
#include <catch.hpp>
#include <document/SceneDocument.h>

using namespace novelist;

TEST_CASE("SceneDocument read/write", "[DataStructures][Document]")
{
    SceneDocument doc;
    doc.setPlainText("This is some plain text.\nIt also has another block.");
    QString xml;
    REQUIRE(doc.write(xml));

    SceneDocument docTest;
    REQUIRE(docTest.read(xml));
    REQUIRE(doc.toPlainText() == docTest.toPlainText());

    //TODO: More in-depth testing
}

TEST_CASE("SceneDocument Cursor test", "[DataStructures][Document]")
{
    SceneDocument doc;
    doc.setPlainText("This is some plain text.\nIt also has another block.");

    QTextCursor cursor1{&doc};
    QTextCursor cursor2{&doc};
    QTextCursor cursor3{&doc};

    cursor2.setPosition(10);
    cursor3.movePosition(QTextCursor::End);

    REQUIRE(doc.characterAt(cursor1.position()) == QChar{'T'});
    REQUIRE(doc.characterAt(cursor2.position()) == QChar{'m'});
    REQUIRE(doc.characterAt(cursor3.position()) == QChar{QChar::ParagraphSeparator});

    cursor1.insertText("Whoa. ");

    REQUIRE(doc.characterAt(cursor1.position()) == QChar{'T'});
    REQUIRE(doc.characterAt(cursor2.position()) == QChar{'m'});
    REQUIRE(doc.characterAt(cursor3.position()) == QChar{QChar::ParagraphSeparator});

    cursor2.insertText(" aweso");

    REQUIRE(doc.characterAt(cursor1.position()) == QChar{'T'});
    REQUIRE(doc.characterAt(cursor2.position()) == QChar{'m'});
    REQUIRE(doc.characterAt(cursor3.position()) == QChar{QChar::ParagraphSeparator});
}