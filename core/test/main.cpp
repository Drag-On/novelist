/**********************************************************
 * @file   main.cpp
 * @author jan
 * @date   16.06.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#define CATCH_CONFIG_RUNNER
#include <catch.hpp>
#include "test/TestApplication.h"

using namespace novelist;

int main(int argc, char** argv)
{
    TestApplication app(argc, argv);

    int result = Catch::Session().run( argc, argv );

    return ( result < 0xff ? result : 0xff );
}