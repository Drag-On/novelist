/**********************************************************
 * @file   LoggingTest.cpp
 * @author jan
 * @date   3/14/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <catch.hpp>
#include <util/Logging.h>

using namespace novelist::log;

TEST_CASE("type_name")
{
    REQUIRE(type_name<int>() == "int");
    REQUIRE(type_name<long int>() == "long int");
    REQUIRE(type_name<unsigned int>() == "unsigned int");
    REQUIRE(type_name<int const>() == "const int");
    REQUIRE(type_name<int*>() == "int*");
}