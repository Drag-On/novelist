/**********************************************************
 * @file   TextAnalyzerTest.cpp
 * @author jan
 * @date   11/25/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <catch.hpp>
#include <test/TestApplication.h>
#include <TextAnalyzer.h>

using namespace novelist;

TEST_CASE("TextAnalyzer", "[TextAnalyzer][String]")
{
    DATA_SECTION("Stats correctness",
            TESTFUN([](QString const& text, size_t charCount, size_t wordCount) {
                auto result = TextAnalyzer::analyze(text);
                REQUIRE(result.m_charCount == charCount);
                REQUIRE(result.m_wordCount == wordCount);
            }),
            NAMED_ROW("Empty string", QStringLiteral(""), 0, 0)
            NAMED_ROW("Single character", QStringLiteral("g"), 1, 1)
            NAMED_ROW("One word", QStringLiteral("Hello"), 5, 1)
            NAMED_ROW("Two words", QStringLiteral("Hello world"), 11, 2)
            NAMED_ROW("Sentence", QStringLiteral("Hello, world!"), 13, 2)
            NAMED_ROW("Numbers", QStringLiteral("6 7 8 Number 9"), 14, 5)
            NAMED_ROW("Umlauts", QStringLiteral("Hallöchen, Welt!"), 16, 2)
            NAMED_ROW("Kanji", QStringLiteral("漢字"), 2, 1)
            NAMED_ROW("Hiragana", QStringLiteral("あなたの なまえは なんですか。"), 16, 3)
            NAMED_ROW("French", QStringLiteral("Que vous êtes joli! Cette leçon vaut bien un fromage."), 53, 10)
            NAMED_ROW("Symbols", QStringLiteral("❤☀ ☯♞♫"), 6, 0)
    )

    SECTION("Combine") {
        auto r1 = TextAnalyzer::analyze(QStringLiteral("What a day!"));
        auto r2 = TextAnalyzer::analyze(QStringLiteral("Herr Lührmann süffelte öfter säuerlichen Saft."));
        auto r3 = TextAnalyzer::analyze(QStringLiteral("Lorem ipsum"));

        auto result = TextAnalyzer::combine(r1, r2, r3);
        REQUIRE(result.m_charCount == r1.m_charCount + r2.m_charCount + r3.m_charCount);
        REQUIRE(result.m_wordCount == r1.m_wordCount + r2.m_wordCount + r3.m_wordCount);
    }
}