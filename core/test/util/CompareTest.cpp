/**********************************************************
 * @file   CompareTest.cpp
 * @author jan
 * @date   3/8/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <catch.hpp>
#include <gsl/gsl>
#include <util/Compare.h>
#include <test/TestApplication.h>

using namespace novelist::compare;

namespace {
    template<typename F, std::size_t... s>
    constexpr void static_for(F&& function, std::index_sequence<s...>)
    {
        [[maybe_unused]] int unpack[] = { 0, (function(std::integral_constant<std::size_t, s>{}), 0)... };
    }

    template<std::size_t iterations, typename F>
    constexpr void static_for(F&& function)
    {
        static_for(std::forward<F>(function), std::make_index_sequence<iterations>());
    }

    template <size_t i> struct test_types;
    template <> struct test_types<0> { using type = uint8_t; };
    template <> struct test_types<1> { using type = uint16_t; };
    template <> struct test_types<2> { using type = uint32_t; };
    template <> struct test_types<3> { using type = uint64_t; };
    template <> struct test_types<4> { using type = int8_t; };
    template <> struct test_types<5> { using type = int16_t; };
    template <> struct test_types<6> { using type = int32_t; };
    template <> struct test_types<7> { using type = int64_t; };
    template <> struct test_types<8> { using type = float; };
    template <> struct test_types<9> { using type = double; };

    constexpr size_t const num_test_types = 10;
}

TEST_CASE("Compare", "[util]")
{
    DATA_SECTION("less",
                TESTFUN([](int a, int b) {
                    INFO("a: " << a << ", b: " << b);
                    static_for<num_test_types>([a, b](auto i) {
                        // If the number can't be cast to the requested type without loss of precision, then gsl::narrow
                        // will throw. This is used here to make sure negative numbers don't wrap into very large
                        // positive numbers when cast to an unsigned type. This would make the test fail. Instead, these
                        // cases are simply ignored.
                        try {
                            auto const firstVal = gsl::narrow<typename test_types<i>::type>(a);
                            INFO("First val type index: " << i);
                            static_for<num_test_types>([firstVal, b](auto j) {
                                try {
                                    auto const secondVal = gsl::narrow<typename test_types<j>::type>(b);
                                    INFO("Second val type index: " << j);
                                    REQUIRE(less(firstVal, secondVal));
                                    REQUIRE_FALSE(less(secondVal, firstVal));
                                }
                                catch(...) {}
                            });
                        }
                        catch(...) {}
                    });
                }),
                ROW(0, 1)
                ROW(-1, 0)
                ROW(0, 1)
                ROW(-1, 1)
                ROW(-2, -1)
                ROW(1, 2)
                )

    DATA_SECTION("equal",
                 TESTFUN([](int a, int b) {
                     static_for<num_test_types>([a, b](auto i) {
                         try {
                             auto const firstVal = gsl::narrow<typename test_types<i>::type>(a);
                             static_for<num_test_types>([firstVal, b](auto j) {
                                 try {
                                     auto const secondVal = gsl::narrow<typename test_types<j>::type>(b);
                                     REQUIRE(equal(firstVal, secondVal));
                                     REQUIRE(equal(secondVal, firstVal));
                                 }
                                 catch (...) {}
                             });
                         }
                         catch (...) {}
                     });
                 }),
                 ROW(0, 0)
                 ROW(1, 1)
                 ROW(-1, -1)
                )

    DATA_SECTION("not equal",
                 TESTFUN([](int a, int b) {
                     static_for<num_test_types>([a, b](auto i) {
                         try {
                             auto const firstVal = gsl::narrow<typename test_types<i>::type>(a);
                             static_for<num_test_types>([firstVal, b](auto j) {
                                 try {
                                     auto const secondVal = gsl::narrow<typename test_types<j>::type>(b);
                                     REQUIRE(not_equal(firstVal, secondVal));
                                     REQUIRE(not_equal(secondVal, firstVal));
                                 }
                                 catch (...) {}
                             });
                         }
                         catch (...) {}
                     });
                 }),
                 ROW(1, 0)
                 ROW(-1, 0)
                )
}