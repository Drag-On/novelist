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

#include "main.h"

bool TestApplication::notify(QObject* receiver, QEvent* e)
{
    try {
        return QApplication::notify(receiver, e);
    }
    catch (std::exception& e) {
        qDebug() << "Caught exception: " << e.what();
        std::terminate();
    }
    catch (...) {
        qDebug() << "Caught unknown exception";
        std::terminate();
    }
}

int main(int argc, char** argv)
{
    TestApplication app(argc, argv);

    int result = Catch::Session().run( argc, argv );

    return ( result < 0xff ? result : 0xff );
}