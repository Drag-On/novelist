/**********************************************************
 * @file   ProjectViewTest.cpp
 * @author jan
 * @date   7/21/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <catch.hpp>
#include <QtWidgets/QApplication>
#include "view/ProjectView.h"

using namespace novelist;

TEST_CASE("ProjectView start", "[.Interactive][View]")
{
    int argc = 0;
    QApplication app(argc, nullptr);

//    ProjectProperties properties{"Foo", "Ernie", Language {"en_US"}};
//    ProjectModel model{properties};
    ProjectView projectView(nullptr);
    projectView.show();

    app.exec();
}