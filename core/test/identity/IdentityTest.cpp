/**********************************************************
 * @file   IdentityTest.cpp
 * @author jan
 * @date   7/20/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <sstream>
#include <catch.hpp>
#include "identity/Identity.h"

using namespace novelist;

struct generic_tag {};
using IdMgr = IdManager<generic_tag>;

TEST_CASE("Id copy & move constraints", "[Identity]")
{
    REQUIRE_FALSE(std::is_copy_assignable_v<IdMgr>);
    REQUIRE_FALSE(std::is_copy_constructible_v<IdMgr>);
    REQUIRE_FALSE(std::is_move_assignable_v<IdMgr>);
    REQUIRE_FALSE(std::is_move_constructible_v<IdMgr>);

    REQUIRE_FALSE(std::is_copy_assignable_v<IdMgr::IdType>);
    REQUIRE_FALSE(std::is_copy_constructible_v<IdMgr::IdType>);
    REQUIRE(std::is_move_assignable_v<IdMgr::IdType>);
    REQUIRE(std::is_move_constructible_v<IdMgr::IdType>);
}

TEST_CASE("Id generation", "[Identity]")
{
    IdMgr manager;

    auto id = manager.generate();
    std::stringstream idss;
    idss << id << std::endl;

    auto id2 = manager.generate();
    std::stringstream id2ss;
    id2ss << id2 << std::endl;

    std::stringstream id3ss;
    {
        auto id3 = manager.generate();
        id3ss << id3 << std::endl;
    }

    auto id4 = manager.generate();
    std::stringstream id4ss;
    id4ss<< id4 << std::endl;

    REQUIRE(id3ss.str() == id4ss.str());
}

TEST_CASE("Id request", "[Identity]")
{
    IdMgr manager;
    auto id = manager.generate();
    auto id2 = manager.generate();

    REQUIRE_NOTHROW(manager.request(10));

    REQUIRE_THROWS(manager.request(0));
}