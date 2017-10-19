/**********************************************************
 * @file   TextEditorTest.cpp
 * @author jan
 * @date   8/1/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <catch.hpp>
#include "main.h"
#include "widgets/texteditor/TextEditor.h"

using namespace novelist;

TEST_CASE("TextEditor start", "[.Interactive][TextEditor][Widget]")
{
    int argc = 0;
    TestApplication app(argc, nullptr);

    TextEditor editor{};

    QString xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<scene version=\"1.0\">\n"
            "    <content>\n"
            "        <block textIndent=\"10\">\n"
            "          <text>This is a </text><text italic=\"1\">crazy</text><text> test. Nobody knows what it is for. And honestly, I don't care.</text>\n"
            "        </block>\n"
            "        <block leftMargin=\"10\" rightMargin=\"10\" topMargin=\"10\" botMargin=\"10\" align=\"8\">\n"
            "          <text capitalization=\"1\" strikeout=\"1\">Foo</text>\n"
            "          <text underline=\"1\" weight=\"75\"> Foobar</text>\n"
            "          <text>Lorem ipsum dolor sit amet, consectetur adipisici elit, sed eiusmod tempor incidunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquid ex ea commodi consequat. Quis aute iure reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint obcaecat cupiditat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.</text>\n"
            "        </block>\n"
            "    </content>\n"
            "</novel>";

    SceneDocument doc;
    if(!doc.read(xml))
        qDebug() << "Read failed";
    editor.setDocument(&doc);
    editor.show();

    app.exec();

    QString outXml;
    doc.write(outXml);
    qDebug() << outXml;
}