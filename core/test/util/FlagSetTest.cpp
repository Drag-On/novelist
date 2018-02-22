/**********************************************************
 * @file   FlagSetTest.cpp
 * @author jan
 * @date   2/21/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <catch.hpp>
#include "util/FlagSet.h"

using namespace novelist;

enum class TestEnum {
    Value1,
    Value2,
    Value3,
    Value4,
    Value5,
};

TEST_CASE("FlagSet", "[Util]")
{
    using Flags = FlagSet<TestEnum>;

    Flags flagset{TestEnum::Value1, TestEnum::Value4};

    SECTION("test") {
        SECTION("testAnyOf") {
            REQUIRE(flagset.testAnyOf(TestEnum::Value1, TestEnum::Value4));
            REQUIRE(flagset.testAnyOf(TestEnum::Value1));
            REQUIRE(flagset.testAnyOf(TestEnum::Value4));
            REQUIRE(flagset.testAnyOf(TestEnum::Value1, TestEnum::Value2));
            REQUIRE_FALSE(flagset.testAnyOf());
            REQUIRE_FALSE(flagset.testAnyOf(TestEnum::Value2));
            REQUIRE_FALSE(flagset.testAnyOf(TestEnum::Value2, TestEnum::Value5));
        }

        SECTION("testAllOf") {
            REQUIRE(flagset.testAllOf(TestEnum::Value1, TestEnum::Value4));
            REQUIRE(flagset.testAllOf(TestEnum::Value1));
            REQUIRE(flagset.testAllOf(TestEnum::Value4));
            REQUIRE(flagset.testAllOf());
            REQUIRE_FALSE(flagset.testAllOf(TestEnum::Value1, TestEnum::Value4, TestEnum::Value5));
        }

        SECTION("testNoneOf") {
            REQUIRE(flagset.testNoneOf(TestEnum::Value2, TestEnum::Value3));
            REQUIRE(flagset.testNoneOf(TestEnum::Value5));
            REQUIRE(flagset.testNoneOf());
            REQUIRE_FALSE(flagset.testNoneOf(TestEnum::Value1, TestEnum::Value2));
            REQUIRE_FALSE(flagset.testNoneOf(TestEnum::Value1, TestEnum::Value4));
        }

        SECTION("testAny") {
            REQUIRE(Flags{TestEnum::Value2}.testAny());
            REQUIRE(Flags{TestEnum::Value2, TestEnum::Value5}.testAny());
            REQUIRE_FALSE(Flags{}.testAny());
        }

        SECTION("testAll") {
            // The following is misleading since it doesn't just check for all enum values, but for all possible bits
            //REQUIRE(Flags{TestEnum::Value1, TestEnum::Value2, TestEnum::Value3, TestEnum::Value4, TestEnum::Value5}.testAll());
            REQUIRE_FALSE(Flags{TestEnum::Value2, TestEnum::Value5}.testAll());
            REQUIRE_FALSE(Flags{}.testAll());
        }

        SECTION("testNone") {
            REQUIRE(Flags{}.testNone());
            REQUIRE_FALSE(Flags{TestEnum::Value2, TestEnum::Value3}.testNone());
        }
    }

    SECTION("clear") {
        flagset.clear(TestEnum::Value1, TestEnum::Value2);
        REQUIRE(flagset.testNoneOf(TestEnum::Value1, TestEnum::Value2));
        REQUIRE(flagset.testAllOf(TestEnum::Value4));

        flagset.clear();
        REQUIRE(flagset.testNone());
    }

    SECTION("set") {
        flagset.clear();
        REQUIRE_FALSE(flagset.test(TestEnum::Value1));
        flagset.set(TestEnum::Value1);
        REQUIRE(flagset.test(TestEnum::Value1));
        flagset.set(TestEnum::Value1);
        REQUIRE(flagset.test(TestEnum::Value1));
    }

    SECTION("toggle") {
        flagset.clear();
        REQUIRE_FALSE(flagset.testAllOf(TestEnum::Value1, TestEnum::Value2));
        flagset.toggle(TestEnum::Value1);
        REQUIRE(flagset.test(TestEnum::Value1));
        REQUIRE_FALSE(flagset.test(TestEnum::Value2));
        flagset.toggle(TestEnum::Value1, TestEnum::Value2);
        REQUIRE_FALSE(flagset.test(TestEnum::Value1));
        REQUIRE(flagset.test(TestEnum::Value2));
    }

}