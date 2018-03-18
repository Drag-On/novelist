/**********************************************************
 * @file   TextCursorHelperTest.cpp
 * @author jan
 * @date   3/18/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <catch.hpp>
#include <editor/document/Document.h>
#include <editor/document/TextCursorHelper.h>
#include <editor/document/TextCursor.h>

using namespace novelist::editor;

TEST_CASE("TextCursorHelper", "[editor][document]")
{
    TextFormatManager mgr;
    auto const parFormatId = mgr.push_back(TextFormatData{"Paragraph"});
    auto const thoughtFormatId = mgr.push_back(TextFormatData{"Thoughts"});
    auto const poetryFormatId = mgr.push_back(TextFormatData{"Poetry"});
    Document doc(&mgr, "Test", getProjectLanguage(Language::English, Country::UnitedStates));
    TextCursor cursor(&doc);
    cursor.insertText("Hello, world!");
    cursor.breakLine();
    cursor.insertText("Another line.");
    cursor.breakParagraph();
    cursor.insertText("Next paragraph.");

    int const start = 0;
    int const endLine1 = 13;
    int const startLine2 = 14;
    int const endLine2 = 27;
    int const startPar2 = 28;
    int const end = 43;

    SECTION("overlappingFragments / setCharacterFormats")
    {
        REQUIRE(helper::overlappingFragments(doc, start, end).size() == 1);

        std::vector<helper::FragmentData> fragments = {
                helper::FragmentData{start, startLine2, parFormatId},
                helper::FragmentData{startLine2, endLine2, thoughtFormatId},
                helper::FragmentData{startPar2, startPar2 + 4, poetryFormatId},
                helper::FragmentData{startPar2 + 4, end, parFormatId}
        };

        helper::setCharacterFormats(doc, fragments);
        auto newFragments = helper::overlappingFragments(doc, start, end);

        REQUIRE(fragments == newFragments);

        int const overwriteStart = 16;
        int const overwriteEnd = 39;

        helper::setCharacterFormats(doc, {{overwriteStart, overwriteEnd, parFormatId}});
        newFragments = helper::overlappingFragments(doc, start, end);

        std::vector<helper::FragmentData> expected = {{start, startLine2, parFormatId},
                                                      {startLine2, overwriteStart, thoughtFormatId},
                                                      {overwriteStart, end, parFormatId}};
        REQUIRE(newFragments == expected);

    }
}