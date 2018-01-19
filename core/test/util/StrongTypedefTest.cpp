/**********************************************************
 * @file   StrongTypedefTest.cpp
 * @author jan
 * @date   1/19/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <catch.hpp>
#include "util/StrongTypedef.h"

using namespace novelist;

TEST_CASE("Strong typedef basic type", "[StrongTypedef]")
{
    using T = int;
    STRONG_TYPEDEF(Id, T);
    Id id;
    REQUIRE(static_cast<T>(id) == T{});

    id = static_cast<Id>(5);
    REQUIRE(static_cast<T>(id) == 5);

    REQUIRE(id == Id(5));
    REQUIRE(id != Id(8));
    REQUIRE(id < Id(6));
    REQUIRE(id <= Id(6));
    REQUIRE(id > Id(4));
    REQUIRE(id >= Id(4));
}

TEST_CASE("Strong typedef multiple same type", "[StrongTypedef]")
{
    using T = int;
    STRONG_TYPEDEF(Id, T);
    STRONG_TYPEDEF(Id2, T);

    Id id{0};
    Id2 id2{0};

    // Doesn't compile:
//    id = id2;
//    id == id2;
}