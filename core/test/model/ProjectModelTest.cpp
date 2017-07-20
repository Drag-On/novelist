/**********************************************************
 * @file   ProjectModelTest.cpp
 * @author jan
 * @date   7/16/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <sstream>
#include <catch.hpp>
#include "model/ProjectModel.h"

using namespace novelist;

using NodeType = ProjectModel::NodeType;

// Default properties for most tests
ProjectProperties properties{"Foo", "Ernie", Language {"en_US"}};

// Some sample data to use in tests
std::map<std::vector<int>, std::pair<char const*, NodeType>> defaultNodes{
        // Project root
        {{0, 0},       {"The Raven",              NodeType::Chapter}},
        {{0, 0, 0},    {"What a Kind King Wants", NodeType::Scene}},
        {{0, 0, 1},    {"Breaking the Chains",    NodeType::Chapter}},
        {{0, 0, 1, 0}, {"Morning Dew",            NodeType::Scene}},
        {{0, 0, 1, 1}, {"Smith the Smith",        NodeType::Scene}},
        {{0, 1},       {"A New Season",           NodeType::Chapter}},
        {{0, 2},       {"Epilogue",               NodeType::Scene}},
        // Notebook
        {{1, 0},       {"Characters",             NodeType::Chapter}}
};

QModelIndex getParIdx(std::vector<int> const& idx, ProjectModel& model)
{
    QModelIndex curIndex = model.projectRootIndex();
    if (idx[0] != 0)
        curIndex = model.notebookIndex();
    for (size_t i = 1; i < idx.size() - 1; ++i)
        curIndex = curIndex.child(idx[i], 0);
    return curIndex;
}

void fillModel(ProjectModel& model)
{
    for (auto[key, val] : defaultNodes)
        model.insertRow(key.back(), val.second, val.first, getParIdx(key, model));
}

TEST_CASE("ProjectModel properties", "[Model]")
{
    ProjectModel model{properties};

    SECTION("Initial state") {
        REQUIRE(model.projectRootIndex().isValid());
        REQUIRE(model.notebookIndex().isValid());

        std::string projectRootDisplay = model.data(model.projectRootIndex(), Qt::DisplayRole).toString().toStdString();
        REQUIRE(projectRootDisplay == properties.m_name.toStdString());

        std::string notebookDisplay = model.data(model.notebookIndex(), Qt::DisplayRole).toString().toStdString();
        REQUIRE(notebookDisplay == "Notebook");
    }

    ProjectProperties newProperties{"Bar", "Bert", Language {"en_UK"}};
    model.setProperties(newProperties);
    SECTION("After change") {
        REQUIRE(model.projectRootIndex().isValid());
        REQUIRE(model.notebookIndex().isValid());

        std::string projectRootDisplay = model.data(model.projectRootIndex(), Qt::DisplayRole).toString().toStdString();
        REQUIRE(projectRootDisplay == newProperties.m_name.toStdString());

        std::string notebookDisplay = model.data(model.notebookIndex(), Qt::DisplayRole).toString().toStdString();
        REQUIRE(notebookDisplay == "Notebook");
    }
}

TEST_CASE("ProjectModel data storage", "[Model]")
{

    SECTION("insertion") {
        ProjectModel model{properties};

        SECTION("Top level") {
            bool result = model.insertRow(0, NodeType::Scene, "Foobar", model.projectRootIndex());
            REQUIRE(result == true);

            std::string insertedDisplay = model.data(model.projectRootIndex().child(0, 0),
                    Qt::DisplayRole).toString().toStdString();
            REQUIRE(insertedDisplay == "Foobar");
        }

        SECTION("nested") {
            bool result = model.insertRow(0, NodeType::Chapter, "Foobar", model.projectRootIndex());
            REQUIRE(result == true);

            std::string insertedDisplay = model.data(model.projectRootIndex().child(0, 0),
                    Qt::DisplayRole).toString().toStdString();
            REQUIRE(insertedDisplay == "Foobar");

            result = model.insertRow(0, NodeType::Scene, "Baz", model.projectRootIndex().child(0, 0));
            REQUIRE(result == true);

            insertedDisplay = model.data(model.projectRootIndex().child(0, 0).child(0, 0),
                    Qt::DisplayRole).toString().toStdString();
            REQUIRE(insertedDisplay == "Baz");
        }
    }

    ProjectModel model{properties};
    fillModel(model);

    for (auto[key, val] : defaultNodes) {
        auto idx = getParIdx(key, model);
        REQUIRE(idx.isValid());

        std::string insertedDisplay = model.data(idx.child(key.back(), 0), Qt::DisplayRole).toString().toStdString();
        REQUIRE(insertedDisplay == val.first);
    }

    SECTION("Bad inserts") {
        REQUIRE_FALSE(model.insertRow(0, NodeType::Scene, "Foo", model.projectRootIndex().child(2, 0)));
        REQUIRE_FALSE(model.insertRow(0, NodeType::Chapter, "Foo", model.projectRootIndex().child(2, 0)));
    }

    SECTION("Move") {
        SECTION("On same level") {
            REQUIRE(model.moveRows(model.projectRootIndex().child(0, 0), 1, 1, model.projectRootIndex().child(0, 0),
                    0));
            std::string display = model.data(model.projectRootIndex().child(0, 0).child(0, 0),
                    Qt::DisplayRole).toString().toStdString();
            std::string correct = defaultNodes[{0, 0, 1}].first;
            REQUIRE(display == correct);
            display = model.data(model.projectRootIndex().child(0, 0).child(1, 0),
                    Qt::DisplayRole).toString().toStdString();
            correct = defaultNodes[{0, 0, 0}].first;
            REQUIRE(display == correct);
        }

        SECTION("Up in the hierarchy") {
            REQUIRE(model.moveRows(model.projectRootIndex().child(0, 0).child(1, 0), 0, 1,
                    model.projectRootIndex().child(0, 0), 1));
            std::string display = model.data(model.projectRootIndex().child(0, 0).child(1, 0),
                    Qt::DisplayRole).toString().toStdString();
            std::string correct = defaultNodes[{0, 0, 1, 0}].first;
            REQUIRE(display == correct);
        }

        SECTION("Down in the hierarchy") {
            REQUIRE(model.moveRows(model.projectRootIndex().child(0, 0), 0, 1,
                    model.projectRootIndex().child(0, 0).child(1, 0), 0));
            std::string display = model.data(model.projectRootIndex().child(0, 0).child(0, 0).child(0, 0),
                    Qt::DisplayRole).toString().toStdString();
            std::string correct = defaultNodes[{0, 0, 0}].first;
            REQUIRE(display == correct);
        }

    }
}

TEST_CASE("ProjectModel read/write", "[Model]")
{
    ProjectModel model{properties};
    fillModel(model);
    std::stringstream modelString;
    modelString << model;

    QString xml;
    REQUIRE(model.write(xml));

    model.clear();

    REQUIRE(model.read(xml));

    std::stringstream modelAfterString;
    modelAfterString << model;

    // Note: Comparison of string representation because implementation details such as IDs might change between loads
    REQUIRE(modelString.str() == modelAfterString.str());

}