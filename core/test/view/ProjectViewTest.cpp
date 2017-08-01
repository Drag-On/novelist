/**********************************************************
 * @file   ProjectViewTest.cpp
 * @author jan
 * @date   7/21/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <catch.hpp>
#include "main.h"
#include "view/ProjectView.h"

using namespace novelist;

TEST_CASE("ProjectView start", "[.Interactive][ProjectView][View]")
{
    int argc = 0;
    TestApplication app(argc, nullptr);

    ProjectProperties properties{"Foo", "Ernie", Language::en_US};
    ProjectModel model{properties};
    ProjectView projectView(nullptr);
    projectView.setModel(&model);
    projectView.show();

    app.exec();
}