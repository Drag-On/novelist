/**********************************************************
 * @file   SceneDocumentTest.cpp
 * @author jan
 * @date   8/6/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <catch.hpp>
#include <datastructures/SceneDocument.h>

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