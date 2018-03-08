/**********************************************************
 * @file   CompareTest.cpp
 * @author jan
 * @date   3/8/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <catch.hpp>
#include <util/Compare.h>
#include <cstdint>

using namespace novelist::compare;

TEST_CASE("Compare", "[util]")
{
    SECTION("less")
    {
        REQUIRE(less(0, 1));
        REQUIRE(less(-1, 0));
        REQUIRE(less(-1, 1));
        REQUIRE(less(-1.f, 1));
        REQUIRE(less(-1, 1.f));
        REQUIRE(less(-1.f, 1.f));
        REQUIRE(less(-1, 1u));
        REQUIRE(less(static_cast<int16_t>(-1), static_cast<uint8_t>(1)));
        REQUIRE(less(static_cast<int8_t>(-1), static_cast<uint8_t>(1)));
        REQUIRE(less(-1, static_cast<uint8_t>(1)));
        REQUIRE(less(-1, static_cast<unsigned int>(1)));
        REQUIRE(less(-1, static_cast<unsigned int>(1)));
        REQUIRE(less(static_cast<bigger_int_t>(-1), static_cast<std::make_unsigned_t<bigger_int_t>>(1)));

        REQUIRE_FALSE(less(1, 0));
        REQUIRE_FALSE(less(0, -1));
        REQUIRE_FALSE(less(1, -1));
        REQUIRE_FALSE(less(1, -1.f));
        REQUIRE_FALSE(less(1.f, -1));
        REQUIRE_FALSE(less(1.f, -1.f));
        REQUIRE_FALSE(less(1u, -1));
        REQUIRE_FALSE(less(static_cast<uint8_t>(1), static_cast<int16_t>(-1)));
        REQUIRE_FALSE(less(static_cast<uint8_t>(1), static_cast<int8_t>(-1)));
        REQUIRE_FALSE(less(static_cast<uint8_t>(1), -1));
        REQUIRE_FALSE(less(static_cast<unsigned int>(1), -1));
        REQUIRE_FALSE(less(static_cast<unsigned int>(1), -1));
        REQUIRE_FALSE(less(static_cast<std::make_unsigned_t<bigger_int_t>>(1), static_cast<bigger_int_t>(-1)));
    }

    SECTION("equal")
    {
        REQUIRE(equal(0, 0));
        REQUIRE(equal(0u, 0));
        REQUIRE(equal(0, 0u));
        REQUIRE(equal(0ul, 0));
        REQUIRE(equal(0, 0ul));
        REQUIRE(equal(static_cast<int8_t>(0), static_cast<uint8_t>(0)));
        REQUIRE(equal(static_cast<int8_t>(0), static_cast<uint16_t>(0)));
        REQUIRE(equal(static_cast<int8_t>(0), static_cast<uint32_t>(0)));
        REQUIRE(equal(static_cast<int8_t>(0), static_cast<uint64_t>(0)));
        REQUIRE(equal(static_cast<int16_t>(0), static_cast<uint8_t>(0)));
        REQUIRE(equal(static_cast<int32_t>(0), static_cast<uint8_t>(0)));
        REQUIRE(equal(static_cast<int64_t>(0), static_cast<uint8_t>(0)));

        REQUIRE_FALSE(equal(0, 1));
        REQUIRE_FALSE(equal(0u, 1));
        REQUIRE_FALSE(equal(0, 1u));
        REQUIRE_FALSE(equal(0ul, 1));
        REQUIRE_FALSE(equal(0, 1ul));
        REQUIRE_FALSE(equal(static_cast<int8_t>(0), static_cast<uint8_t>(1)));
        REQUIRE_FALSE(equal(static_cast<int8_t>(0), static_cast<uint16_t>(1)));
        REQUIRE_FALSE(equal(static_cast<int8_t>(0), static_cast<uint32_t>(1)));
        REQUIRE_FALSE(equal(static_cast<int8_t>(0), static_cast<uint64_t>(1)));
        REQUIRE_FALSE(equal(static_cast<int16_t>(0), static_cast<uint8_t>(1)));
        REQUIRE_FALSE(equal(static_cast<int32_t>(0), static_cast<uint8_t>(1)));
        REQUIRE_FALSE(equal(static_cast<int64_t>(0), static_cast<uint8_t>(1)));
    }
}