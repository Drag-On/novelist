/**********************************************************
 * @file   TreeTest.cpp.cpp
 * @author jan
 * @date   7/11/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <catch.hpp>
#include "datastructures/Tree.h"

using namespace novelist;

TEST_CASE("TreeNode parent validity", "[DataStructures][Tree]")
{
    TreeNode<int> node{3};
    SECTION("no parent")
    {
        REQUIRE(node.parent() == nullptr);
    }

    auto const& child = node.emplace_back(42);
    SECTION("valid parent")
    {
        REQUIRE(child.parent() == &node);
        REQUIRE(node.parent() == nullptr);

        auto const& other_child = node.emplace_back(24);
        REQUIRE(other_child.parent() == &node);
        REQUIRE(node.parent() == nullptr);
    }
}

TEST_CASE("TreeNode clone", "[DataStructures][Tree]")
{
    TreeNode<int> node{3};
    auto clone = node.clone();
    REQUIRE(node.size() == clone.size());
    REQUIRE(node.m_data == clone.m_data);

    auto& c1 = node.emplace_back(42);
    c1.emplace_back(12);
    auto const& c2 = node.emplace_back(13);
    clone = node.clone();

    REQUIRE(node.size() == clone.size());
    REQUIRE(node.m_data == clone.m_data);
    REQUIRE(node[0].size() == clone[0].size());
    REQUIRE(node[0].m_data == clone[0].m_data);
    REQUIRE(node[1].size() == clone[1].size());
    REQUIRE(node[1].m_data == clone[1].m_data);
    REQUIRE(node[0][0].size() == clone[0][0].size());
    REQUIRE(node[0][0].m_data == clone[0][0].m_data);
}

TEST_CASE("TreeNode take children", "[DataStructures][Tree]")
{
    TreeNode<int> node{3};
    node.emplace_back(42);
    node.emplace_back(34);
    node[0].emplace_back(16);

    auto iter = node.begin();
    auto copy = iter->clone();

    auto child1Taken = node.take(iter);
    REQUIRE(child1Taken.parent() == nullptr);
    REQUIRE(child1Taken.size() == copy.size());
    REQUIRE(child1Taken.m_data == copy.m_data);
}

TEST_CASE("TreeNode move children", "[DataStructures][Tree]")
{
    TreeNode<int> node{1};
    node.emplace_back(11);
    node[0].emplace_back(111);
    node[0].emplace_back(112);
    node[0][1].emplace_back(1121);
    node[0][1].emplace_back(1122);
    node[0].emplace_back(113);
    node.emplace_back(12);
    node[1].emplace_back(121);
    node[1][0].emplace_back(1211);
    node[1][0].emplace_back(1212);
    node[1].emplace_back(122);

    SECTION("Same level")
    {
        SECTION("No-op")
        {
            bool success = node[0].move(0, node[0], 0);
            REQUIRE(success);
            REQUIRE(node[0][0].m_data == 111);
            REQUIRE(node[0][1].m_data == 112);
            REQUIRE(node[0][2].m_data == 113);

            success = node[0].move(0, node[0], 1);
            REQUIRE(success);
            REQUIRE(node[0][0].m_data == 111);
            REQUIRE(node[0][1].m_data == 112);
            REQUIRE(node[0][2].m_data == 113);
        }

        SECTION("Top to bottom")
        {
            bool success = node[0].move(0, node[0], 2);
            REQUIRE(success);
            REQUIRE(node[0][0].m_data == 112);
            REQUIRE(node[0][1].m_data == 113);
            REQUIRE(node[0][2].m_data == 111);
        }

        SECTION("Bottom to top")
        {
            bool success = node[0].move(2, node[0], 0);
            REQUIRE(success);
            REQUIRE(node[0][0].m_data == 113);
            REQUIRE(node[0][1].m_data == 111);
            REQUIRE(node[0][2].m_data == 112);
        }
    }

    SECTION("One level up")
    {
        SECTION("Before src node")
        {
            bool success = node[0].move(1, node, 0);
            REQUIRE(success);
            REQUIRE(node[0].m_data == 112);
            REQUIRE(node[1].m_data == 11);
            REQUIRE(node[2].m_data == 12);
            REQUIRE(node[1].size() == 2);
            REQUIRE(node[1][0].m_data == 111);
            REQUIRE(node[1][1].m_data == 113);
        }

        SECTION("After src node")
        {
            bool success = node[0].move(1, node, 1);
            REQUIRE(success);
            REQUIRE(node[0].m_data == 11);
            REQUIRE(node[0][0].m_data == 111);
            REQUIRE(node[0][1].m_data == 113);
            REQUIRE(node[1].m_data == 112);
            REQUIRE(node[1][0].m_data == 1121);
            REQUIRE(node[1][1].m_data == 1122);
            REQUIRE(node[2].m_data == 12); }
    }

    SECTION("One level down")
    {
        SECTION("Down itself (should fail)")
        {
            bool success = node.move(0, node[0], 0);
            REQUIRE_FALSE(success);
        }

        SECTION("Down a sibling node")
        {
            SECTION("Sibling before src")
            {
                bool success = node.move(0, node[1], 0);
                REQUIRE(success);
                REQUIRE(node[0].m_data == 12);
                REQUIRE(node[0][0].m_data == 11);
                REQUIRE(node[0][1].m_data == 121);
                REQUIRE(node[0][2].m_data == 122);
            }

            SECTION("Sibling after src")
            {
                bool success = node.move(1, node[0], 0);
                REQUIRE(success);
                REQUIRE(node[0].m_data == 11);
                REQUIRE(node[0][0].m_data == 12);
                REQUIRE(node[0][1].m_data == 111);
                REQUIRE(node[0][2].m_data == 112);
                REQUIRE(node[0][3].m_data == 113);
            }
        }
    }

    SECTION("Between trees")
    {
        TreeNode<int> otherNode{0};
        bool success = node[0].move(1, otherNode, 0);
        REQUIRE(success);
        REQUIRE(node[0].m_data == 11);
        REQUIRE(node[0][0].m_data == 111);
        REQUIRE(node[0][1].m_data == 113);
        REQUIRE(node[1].m_data == 12);

        REQUIRE(otherNode.m_data == 0);
        REQUIRE(otherNode[0].m_data == 112);
        REQUIRE(otherNode[0][0].m_data == 1121);
        REQUIRE(otherNode[0][1].m_data == 1122);
    }
}

TEST_CASE("TreeNode traverse with DFS", "[DataStructures][Tree]")
{
    auto check = [](TreeNode<int> const& root, std::vector<int> const& expected)
    {
        size_t i = 0;
        auto checker = [&](TreeNode<int> const& n)
        {
            REQUIRE(i < expected.size());
            REQUIRE(n.m_data == expected[i]);
            ++i;
            return false;
        };
        traverse_dfs(root, checker);
    };

    TreeNode<int> node{3};

    SECTION("Just root node")
    {
        std::vector<int> expected {3};
        check(node, expected);
    }

    node.emplace_back(7).emplace_back(8);
    node[0].emplace_back(9);
    node.emplace_back(12).emplace_back(109);

    SECTION("With tree")
    {
        std::vector<int> expected {3, 7, 8, 9, 12, 109};
        check(node, expected);
    }

    SECTION("Find existing element")
    {
        auto const* found = traverse_dfs(node, [](TreeNode<int> const& n){return n.m_data == 9;});
        REQUIRE(found != nullptr);
        REQUIRE(found->m_data == 9);
    }

    SECTION("Try find nonexisting element")
    {
        auto const* found = traverse_dfs(node, [](TreeNode<int> const& n){return n.m_data == 9999;});
        REQUIRE(found == nullptr);
    }
}

TEST_CASE("TreeNode traverse with BFS", "[DataStructures][Tree]")
{
    auto check = [](TreeNode<int> const& root, std::vector<int> const& expected)
    {
        size_t i = 0;
        auto checker = [&](TreeNode<int> const& n)
        {
            REQUIRE(i < expected.size());
            REQUIRE(n.m_data == expected[i]);
            ++i;
            return false;
        };
        traverse_bfs(root, checker);
    };

    TreeNode<int> node{3};

    SECTION("Just root node")
    {
        std::vector<int> expected {3};
        check(node, expected);
    }

    node.emplace_back(7).emplace_back(8);
    node[0].emplace_back(9);
    node.emplace_back(12).emplace_back(109);

    SECTION("With tree")
    {
        std::vector<int> expected {3, 7, 12, 8, 9, 109};
        check(node, expected);
    }

    SECTION("Find existing element")
    {
        auto const* found = traverse_bfs(node, [](TreeNode<int> const& n){return n.m_data == 9;});
        REQUIRE(found != nullptr);
        REQUIRE(found->m_data == 9);
    }

    SECTION("Try find nonexisting element")
    {
        auto const* found = traverse_bfs(node, [](TreeNode<int> const& n){return n.m_data == 9999;});
        REQUIRE(found == nullptr);
    }
}

TEST_CASE("TreeNode is child of", "[DataStructures][Tree]")
{
    TreeNode<int> node{3};
    TreeNode<int> other{6};
    node.emplace_back(8);
    node[0].emplace_back(7);
    node.emplace_back(42);

    SECTION("Failure case")
    {
        REQUIRE_FALSE(node.inSubtreeOf(other));
        REQUIRE_FALSE(node.inSubtreeOf(node));
        REQUIRE_FALSE(node[0].inSubtreeOf(node[1]));
    }

    SECTION("Success case")
    {
        REQUIRE(node[0].inSubtreeOf(node));
        REQUIRE(node.at(0).at(0).inSubtreeOf(node));
        REQUIRE(node[0][0].inSubtreeOf(node[0]));
        REQUIRE(node[1].inSubtreeOf(node));
    }

}
