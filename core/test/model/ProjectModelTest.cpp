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

using NodeType = ProjectModel::InsertableNodeType;

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
        auto testMove = [&](std::vector<int> const& src, std::vector<int> const& dest)
        {
            auto rootIdx = [&](int c)
            {
                if(c == 0)
                    return model.projectRootIndex();
                else if (c == 1)
                    return model.notebookIndex();
                return QModelIndex{};
            };
            auto parIdx = [&](std::vector<int> const& idx)
            {
                auto root = rootIdx(idx.front());
                for(size_t i = 1; i < idx.size() - 1; ++i)
                    root = root.child(idx[i], 0);
                return root;
            };
            auto index = [&](std::vector<int> const& idx)
            {
                auto root = rootIdx(idx.front());
                for(size_t i = 1; i < idx.size(); ++i)
                    root = root.child(idx[i], 0);
                return root;
            };

            std::string srcStr = defaultNodes[src].first;
            std::string destStr = defaultNodes[dest].first;
            std::vector<int> srcPar(src.begin(), src.end()-1);
            std::vector<int> destPar(dest.begin(), dest.end()-1);
            std::string srcParStr = defaultNodes[srcPar].first;
            std::string destParStr = defaultNodes[destPar].first;
            QPersistentModelIndex srcParent{parIdx(src)};
            QPersistentModelIndex destParent{parIdx(dest)};
            QPersistentModelIndex srcIdx{index(src)};
            QPersistentModelIndex destIdx{index(dest)};
            REQUIRE(model.moveRow(srcParent, src.back(), destParent, dest.back()));
            REQUIRE(srcParent.isValid());
            REQUIRE(destParent.isValid());
            REQUIRE(srcIdx.isValid());
            REQUIRE(destIdx.isValid());
            std::string srcParDisp = model.data(srcParent, Qt::DisplayRole).toString().toStdString();
            REQUIRE(srcParDisp == srcParStr);
            std::string destParDisp = model.data(destParent, Qt::DisplayRole).toString().toStdString();
            REQUIRE(destParDisp == destParStr);
            std::string srcDisp = model.data(srcIdx, Qt::DisplayRole).toString().toStdString();
            REQUIRE(srcDisp == srcStr);
            std::string destDisp = model.data(destIdx, Qt::DisplayRole).toString().toStdString();
            REQUIRE(destDisp == destStr);
        };

        SECTION("On same level") {
            SECTION("End to beginning")
            {
                testMove({0, 0, 1}, {0, 0, 0});
            }

            SECTION("Beginning to end")
            {
                testMove({0, 0, 0}, {0, 0, 1});
            }
        }

        SECTION("Up in the hierarchy") {
            testMove({0, 0, 1, 0}, {0, 0, 1});
        }

        SECTION("Down in the hierarchy") {
            testMove({0, 0, 0}, {0, 0, 1, 0});
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