/**********************************************************
 * @file   IdentityTest.cpp
 * @author jan
 * @date   7/20/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <sstream>
#include <random>
#include <set>
#include <catch.hpp>
#include "identity/Identity.h"

using namespace novelist;

struct generic_tag {};
using IdMgr = IdManager<generic_tag>;
using IdType = IdMgr::IdType;

bool checkIdsUnique(std::vector<IdType> const& ids)
{
    std::set<int> found;
    for(auto const& id : ids) {
        if(found.count(id.id()) > 0)
            return false;

        found.insert(id.id());
    }
    return true;
}

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
    std::vector<IdType> ids;

    for(int i = 0; i < 50; ++i)
        ids.emplace_back(manager.generate());

    REQUIRE(checkIdsUnique(ids));

    static std::random_device rd;
    static std::mt19937 gen(rd());
    for(int i = 0; i < 10; ++i) {
        std::uniform_int_distribution<> dis(0, ids.size() - 1);
        ids.erase(ids.begin() + dis(gen));
    }

    REQUIRE(checkIdsUnique(ids));

    for(int i = 0; i < 50; ++i)
        ids.emplace_back(manager.generate());

    REQUIRE(checkIdsUnique(ids));
}

TEST_CASE("Id request", "[Identity]")
{
    IdMgr manager;
    std::vector<IdType> ids;

    for(int i = 0; i < 50; ++i)
        ids.emplace_back(manager.generate());

    REQUIRE_NOTHROW(manager.request(60));

    REQUIRE_THROWS(manager.request(0));

    REQUIRE(checkIdsUnique(ids));

    for(int i = 0; i < 50; ++i)
        ids.emplace_back(manager.generate());

    REQUIRE(checkIdsUnique(ids));
}