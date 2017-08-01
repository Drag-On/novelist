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
#include "widgets/TextEditor.h"

using namespace novelist;

TEST_CASE("TextEditor start", "[.Interactive][TextEditor][Widget]")
{
    int argc = 0;
    TestApplication app(argc, nullptr);

    TextEditor editor{};
    editor.show();

    app.exec();
}