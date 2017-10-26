/**********************************************************
 * @file   SortedVectorTest.cpp
 * @author jan
 * @date   10/26/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <catch.hpp>
#include <datastructures/SortedVector.h>
#include <macros.h>

using namespace novelist;

bool isSorted(SortedVector<int> const& v)
{
    if (v.size() < 2)
        return true;

    for (size_t i = 1; i < v.size(); ++i)
        if (v.at(i) < v.at(i - 1))
            return false;
    return true;
}

TEST_CASE("SortedVector construction", "[DataStructures][SortedVector]")
{
    std::vector<int> unordered{1, 6, 3, 9};

    SECTION("Initializer List") {
        REQUIRE(isSorted(SortedVector<int>{1, 5, 8, 3}));
    }
    SECTION("From element") {
        REQUIRE(isSorted(SortedVector<int>(5)));
        REQUIRE(isSorted(SortedVector<int>(5, 42)));
    }
    SECTION("Copy") {
        SECTION("Copy from std::vector") {
            REQUIRE(isSorted(SortedVector<int>(unordered)));
        }
        SECTION("Copy from std::vector iterators") {
            SortedVector<int> vec(unordered.begin(), unordered.end());
            REQUIRE(isSorted(vec));
        }
        SECTION("Copy from SortedVector") {
            SortedVector<int> sorted{1, 2, 3, 4, 5};
            SortedVector<int> vec(sorted);
            REQUIRE(isSorted(vec));
        }
        SECTION("Copy from SortedVector iterators") {
            SortedVector<int> sorted{1, 2, 3, 4, 5};
            SortedVector<int> vec(sorted.begin(), sorted.end());
            REQUIRE(isSorted(vec));
        }
    }
    SECTION("Move") {
        SECTION("Move from std::vector") {
            SortedVector<int> vec(std::move(unordered));
            REQUIRE(isSorted(vec));
        }
        SECTION("Move from SortedVector") {
            SortedVector<int> sorted{1, 2, 3, 4, 5};
            SortedVector<int> vec(std::move(sorted));
            REQUIRE(isSorted(vec));
        }
    }
    SECTION("Copy Assignment") {
        SortedVector<int> v1{1, 2, 3, 4, 5};
        SortedVector<int> v2{6, 7, 8};
        REQUIRE(isSorted(v1 = v2));
    }
    SECTION("Move Assignment") {
        SortedVector<int> v1{1, 2, 3, 4, 5};
        SortedVector<int> v2{6, 7, 8};
        REQUIRE(isSorted(v1 = std::move(v2)));
    }
    SECTION("Assignment from initializer list") {
        SortedVector<int> v{1, 2, 3, 4, 5};
        REQUIRE(isSorted(v = {6, 7, 8}));
    }
}

TEST_CASE("SortedVector insert", "[DataStructures][SortedVector]")
{
    SortedVector<int> v{-7, 1, 4, 7};

    DATA_SECTION("simple",
            TESTFUN([&v](int i) {
                v.insert(i);
                REQUIRE(isSorted(v));
            }),
            ROW(0)
            ROW(1)
            ROW(10)
            ROW(-1)
            ROW(-100)
    )

    DATA_SECTION("multiples",
            TESTFUN([&v](int i, size_t c) {
                v.insert(c, i);
                REQUIRE(isSorted(v));
            }),
            ROW(0, 1)
            ROW(1, 4)
            ROW(10, 3)
            ROW(-1, 42)
            ROW(-100, 3)
            ROW(17, 0)
    )

    std::vector<int> v2{3, 0, 1, -1};
    DATA_SECTION("range",
            TESTFUN([&v, &v2](int first, int last) {
                v.insert(v2.begin() + first, v2.begin() + last);
                REQUIRE(isSorted(v));
            }),
            ROW(0, 4)
            ROW(0, 1)
            ROW(1, 4)
            ROW(2, 3)
            ROW(1, 1)
    )

    DATA_SECTION("initializer list",
            TESTFUN([&v, &v2](std::initializer_list<int> l) {
                v.insert(l);
                REQUIRE(isSorted(v));
            }),
            ROW({0, 1, 2, 3})
            ROW({-51, 44, 0, 999})
            ROW({87, 8, -424984, 283})
    )
}

TEST_CASE("SortedVector modify", "[DataStructures][SortedVector]")
{
    SortedVector<int> v{-99, -7, 1, 4, 7, 10000};

    DATA_SECTION("First element",
            TESTFUN([&v](int n) {
                auto iter = v.modify(v.begin(), [n](int& e) { e = n; });
                REQUIRE(isSorted(v));
                REQUIRE(*iter == n);
            }),
            NAMED_ROW("lesser", -800)
            NAMED_ROW("same", -99)
            NAMED_ROW("bigger", 0)
            NAMED_ROW("max", 999999)
    )
    DATA_SECTION("Last element",
            TESTFUN([&v](int n) {
                auto iter = v.modify(v.begin() + v.size() - 1, [n](int& e) { e = n; });
                INFO(v);
                REQUIRE(isSorted(v));
                REQUIRE(*iter == n);
            }),
            NAMED_ROW("min", -999999)
            NAMED_ROW("lesser", 0)
            NAMED_ROW("same", 10000)
            NAMED_ROW("bigger", 10900)
    )
    DATA_SECTION("Middle element",
            TESTFUN([&v](int n) {
                auto iter = v.modify(v.begin() + 3, [n](int& e) { e = n; });
                INFO(v);
                REQUIRE(isSorted(v));
                REQUIRE(*iter == n);
            }),
            NAMED_ROW("min", -999999)
            NAMED_ROW("lesser", 0)
            NAMED_ROW("same", 4)
            NAMED_ROW("bigger", 14)
            NAMED_ROW("max", 10900)
    )
}

namespace std {
    std::ostream& operator<<(std::ostream& stream, std::vector<int> const& vec)
    {
        stream << "{ ";
        if (!vec.empty())
            stream << vec.front();
        for (size_t i = 1; i < vec.size(); ++i)
            stream << ", " << vec[i];
        stream << " }";
        return stream;
    }
}

TEST_CASE("vector move_range", "[DataStructures]")
{
    SECTION("No-op") {
        std::vector<int> vec{-99, -7, 1, 4, 7, 10000};
        auto iter = move_range(vec.begin() + 1, 1, vec.begin() + 1);
        REQUIRE(vec == std::vector<int>({-99, -7, 1, 4, 7, 10000}));
        REQUIRE(*iter == -7);

        iter = move_range(vec.begin() + 1, 1, vec.begin() + 2);
        REQUIRE(vec == std::vector<int>({-99, -7, 1, 4, 7, 10000}));
        REQUIRE(*iter == -7);
    }
    SECTION("Front to back") {
        std::vector<int> vec{-99, -7, 1, 4, 7, 10000};
        auto iter = move_range(vec.begin(), 1, vec.end());
        REQUIRE(vec == std::vector<int>({-7, 1, 4, 7, 10000, -99}));
        REQUIRE(*iter == -99);
    }
    SECTION("Back to front") {
        std::vector<int> vec{-99, -7, 1, 4, 7, 10000};
        auto iter = move_range(vec.end()-1, 1, vec.begin());
        REQUIRE(vec == std::vector<int>({10000, -99, -7, 1, 4, 7}));
        REQUIRE(*iter == 10000);
    }
    SECTION("middle forward") {
        std::vector<int> vec{-99, -7, 1, 4, 7, 10000};
        auto iter = move_range(vec.begin() + 2, 1, vec.end() - 1);
        REQUIRE(vec == std::vector<int>({-99, -7, 4, 7, 1, 10000}));
        REQUIRE(*iter == 1);
    }
    SECTION("middle backward") {
        std::vector<int> vec{-99, -7, 1, 4, 7, 10000};
        auto iter = move_range(vec.end() - 2, 1, vec.begin() + 1);
        REQUIRE(vec == std::vector<int>({-99, 7, -7, 1, 4, 10000}));
        REQUIRE(*iter == 7);
    }
    SECTION("adjacent forward") {
        std::vector<int> vec{-99, -7, 1, 4, 7, 10000};
        auto iter = move_range(vec.begin() + 2, 1, vec.end() - 2);
        REQUIRE(vec == std::vector<int>({-99, -7, 4, 1, 7, 10000}));
        REQUIRE(*iter == 1);
    }
    SECTION("adjacent backward") {
        std::vector<int> vec{-99, -7, 1, 4, 7, 10000};
        auto iter = move_range(vec.end() - 3, 1, vec.begin() + 2);
        REQUIRE(vec == std::vector<int>({-99, -7, 4, 1, 7, 10000}));
        REQUIRE(*iter == 4);
    }
}