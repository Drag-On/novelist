/**********************************************************
 * @file   ProjectModelTest.cpp
 * @author jan
 * @date   10/3/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <iostream>
#include <catch.hpp>
#include "model/ProjectModel.h"
#include "macros.h"

using namespace novelist;

using NodeType = ProjectModel::InsertableNodeType;

// Default properties for most tests
ProjectProperties properties{"Foo", "Ernie", Language::en_US};

// Some sample data to use in tests
std::map<ModelPath, std::pair<char const*, NodeType>> const defaultNodes{
        // Project root
        {{0, 0},       {"The Raven",              NodeType::Chapter}},
        {{0, 0, 0},    {"What a Kind King Wants", NodeType::Scene}},
        {{0, 0, 1},    {"Breaking the Chains",    NodeType::Chapter}},
        {{0, 0, 1, 0}, {"Morning Dew",            NodeType::Scene}},
        {{0, 0, 1, 1}, {"Smith the Smith",        NodeType::Scene}},
        {{0, 0, 1, 2}, {"A Kind Gift",            NodeType::Chapter}},
        {{0, 1},       {"A New Season",           NodeType::Chapter}},
        {{0, 1, 0},    {"The War of Thunder",     NodeType::Chapter}},
        {{0, 2},       {"Epilogue",               NodeType::Scene}},
        // Notebook
        {{1, 0},       {"Characters",             NodeType::Chapter}},
        {{1, 1},       {"Locations",              NodeType::Chapter}},
        {{1, 2},       {"Items",                  NodeType::Chapter}}
};

void fillModel(ProjectModel& model)
{
    for (auto const& [path, data] : defaultNodes)
        model.insertRow(path.leaf().first, data.second, data.first, path.parentPath().toModelIndex(&model));
}

TEST_CASE("ProjectModel properties", "[Model]")
{
    ProjectModel model{properties};

    SECTION("Initial state") {
        REQUIRE(model.projectRootIndex().isValid());
        REQUIRE(model.notebookIndex().isValid());

        std::string projectRootDisplay = model.nodeName(model.projectRootIndex()).toStdString();
        REQUIRE(projectRootDisplay == properties.m_name.toStdString());
    }

    DATA_SECTION("Change properties",
        TESTFUN([&model](QString const& title, QString const& author, Language lang) {
            model.setProperties({title, author, lang});
            std::string projectRootDisplay = model.nodeName(model.projectRootIndex()).toStdString();
            REQUIRE(projectRootDisplay == title.toStdString());

            REQUIRE(model.properties().m_name == title);
            REQUIRE(model.properties().m_author == author);
            REQUIRE(model.properties().m_lang == lang);
        }),
        ROW("Bar", "Bert", Language::en_UK)
        ROW("", "Bert", Language::en_UK)
        ROW("Bar", "", Language::en_UK)
        ROW("", "", Language::en_UK)
    )
}

TEST_CASE("ProjectModel insert", "[Model]")
{
    ProjectModel model{properties};
    fillModel(model);

    for (auto const& [path, data] : defaultNodes) {
        auto idx = path.toModelIndex(&model);
        std::string name = model.nodeName(idx).toStdString();
        REQUIRE(name == data.first);
    }

    DATA_SECTION("Bad inserts",
        TESTFUN([&model] (ModelPath const& p, int idx) {
            REQUIRE_FALSE(model.insertRow(idx, NodeType::Scene, "", p.toModelIndex(&model)));
        }),
        NAMED_ROW("scene node", {0, 0, 0}, 0)
        NAMED_ROW("root node", {}, 0)
        NAMED_ROW("nonexistent node", {0, 0, 0, 0, 0, 0, 0}, 0)
        NAMED_ROW("child out of bounds", {0, 0}, 15)
    )

    DATA_SECTION("Good inserts",
        TESTFUN([&model] (ModelPath const& p, int idx) {
            REQUIRE(model.insertRow(idx, NodeType::Scene, "foobar", p.toModelIndex(&model)));
            REQUIRE(model.nodeName(p.toModelIndex(&model).child(idx, 0)).toStdString() == "foobar");
        }),
        NAMED_ROW("In project root (front)", {0}, 0)
        NAMED_ROW("In project root (middle)", {0}, 2)
        NAMED_ROW("In project root (back)", {0}, 3)
        NAMED_ROW("In notebook (front)", {1}, 0)
        NAMED_ROW("In notebook (middle)", {1}, 2)
        NAMED_ROW("In notebook (back)", {1}, 3)
        NAMED_ROW("In chapter (front)", {0, 0}, 0)
        NAMED_ROW("In chapter (middle)", {0, 0}, 1)
        NAMED_ROW("In chapter (back)", {0, 0}, 2)
    )
}

TEST_CASE("ProjectModel remove", "[Model]")
{
    ProjectModel model{properties};
    fillModel(model);

    DATA_SECTION("Bad removals",
        TESTFUN([&model] (ModelPath const& p, int idx) {
            REQUIRE_FALSE(model.removeRow(idx, p.toModelIndex(&model)));
        }),
        NAMED_ROW("project root node", {}, 0)
        NAMED_ROW("notebook node", {}, 1)
        NAMED_ROW("nonexistent node", {0, 0, 0, 0, 0, 0, 0}, 0)
        NAMED_ROW("child out of bounds", {0, 0}, 15)
    )

    DATA_SECTION("Good removals",
        TESTFUN([&model] (ModelPath const& p, int idx) {
            QPersistentModelIndex parentIdx = p.toModelIndex(&model);
            QPersistentModelIndex elementIdx = parentIdx.child(idx, 0);
            REQUIRE(model.removeRow(idx, parentIdx));
            REQUIRE(parentIdx.isValid());
            REQUIRE_FALSE(elementIdx.isValid());
        }),
        NAMED_ROW("In project root (front)", {0}, 0)
        NAMED_ROW("In project root (middle)", {0}, 1)
        NAMED_ROW("In project root (back)", {0}, 2)
        NAMED_ROW("In notebook (front)", {1}, 0)
        NAMED_ROW("In notebook (middle)", {1}, 1)
        NAMED_ROW("In notebook (back)", {1}, 2)
        NAMED_ROW("In chapter (front)", {0, 0, 1}, 0)
        NAMED_ROW("In chapter (middle)", {0, 0, 1}, 1)
        NAMED_ROW("In chapter (back)", {0, 0, 1}, 2)
    )
}

TEST_CASE("ProjectModel move", "[Model]")
{
    ProjectModel model{properties};
    fillModel(model);

    DATA_SECTION("Bad moves",
        TESTFUN([&model] (ModelPath const& src, ModelPath const& dest) {
            auto srcParIdx = src.parentPath().toModelIndex(&model);
            auto destParIdx = dest.parentPath().toModelIndex(&model);
            bool r = model.moveRow(srcParIdx, src.leaf().first, destParIdx, dest.leaf().first);
            REQUIRE_FALSE(r);

            ProjectModel referenceModel{properties};
            fillModel(referenceModel);
            REQUIRE(model == referenceModel);
        }),
        NAMED_ROW("Project root onto itself", {0}, {0})
        NAMED_ROW("Project root into own child", {0}, {0, 0, 0})
        NAMED_ROW("Project root after notebook", {0}, {2})
        NAMED_ROW("Notebook onto itself", {1}, {1})
        NAMED_ROW("Notebook into own child", {1}, {1, 0, 0})
        NAMED_ROW("Notebook before project root", {1}, {0})
        NAMED_ROW("Chapter down into own child", {0, 0}, {0, 0, 0})
        NAMED_ROW("Chapter into scene", {0, 0}, {0, 0, 0, 0})
        NAMED_ROW("To root node", {0, 0}, {0})
        NAMED_ROW("To nonexisting node", {0, 0}, {0, 0, 0, 0, 0, 0, 0, 0})
        NAMED_ROW("From nonexisting node", {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0})
    )

    DATA_SECTION("Good moves",
        TESTFUN([&model] (ModelPath const& src, ModelPath const& dest) {
            auto srcParIdx = src.parentPath().toModelIndex(&model);
            auto destParIdx = dest.parentPath().toModelIndex(&model);
            QPersistentModelIndex persistentIdx = srcParIdx.child(src.leaf().first, 0);
            std::string name = model.nodeName(persistentIdx).toStdString();
            bool r = model.moveRow(srcParIdx, src.leaf().first, destParIdx, dest.leaf().first);
            REQUIRE(r);

            ProjectModel referenceModel{properties};
            fillModel(referenceModel);
            REQUIRE_FALSE(model == referenceModel);

            REQUIRE(model.nodeName(persistentIdx).toStdString() == name);
        }),
        NAMED_ROW("Same level (up)", {0, 0, 1}, {0, 0, 0})
        NAMED_ROW("Same level (down)", {0, 0, 0}, {0, 0, 2})
        NAMED_ROW("Up in hierarchy (from below)", {0, 0, 1, 0}, {0, 0, 1})
        NAMED_ROW("Up in hierarchy (from above)", {0, 0, 1, 0}, {0, 0, 2})
        NAMED_ROW("2x up in hierarchy (from below)", {0, 0, 1, 0}, {0, 0})
        NAMED_ROW("2x up in hierarchy (from above)", {0, 0, 1, 0}, {0, 1})
        NAMED_ROW("Down in hierarchy (from below)", {0, 1}, {0, 0, 1})
        NAMED_ROW("Down in hierarchy (from above)", {0, 0, 0}, {0, 0, 1, 0})
        NAMED_ROW("2x down in hierarchy (from below)", {0, 1}, {0, 0, 1, 2})
        NAMED_ROW("2x down in hierarchy (from above)", {0, 0}, {0, 1, 0, 0})
        NAMED_ROW("Scene into unrelated chapter", {0, 0, 0}, {0, 1, 0})
        NAMED_ROW("Scene to notebook", {0, 0, 0}, {1, 0})
        NAMED_ROW("Chapter to notebook", {0, 0, 1}, {1, 0})
    )
}

TEST_CASE("ProjectModel undo/redo", "[Model]")
{
    ProjectModel model{properties};
    fillModel(model);

    ProjectModel referenceModel{properties};
    fillModel(referenceModel);

    DATA_SECTION("insert",
        TESTFUN([&] (ModelPath const& parent, int row) {
            REQUIRE(model.insertRow(row, NodeType::Scene, "foo", parent.toModelIndex(&model)));
            model.undoStack().undo();
            REQUIRE(model == referenceModel);
            REQUIRE(referenceModel.insertRow(row, NodeType::Scene, "foo", parent.toModelIndex(&referenceModel)));
            model.undoStack().redo();
            REQUIRE(model == referenceModel);
        }),
        NAMED_ROW("In project root", {0}, 0)
        NAMED_ROW("In notebook root", {1}, 0)
        NAMED_ROW("In chapter", {0, 0}, 0)
    )

    DATA_SECTION("remove",
        TESTFUN([&] (ModelPath const& parent, int row) {
            REQUIRE(model.removeRow(row, parent.toModelIndex(&model)));
            model.undoStack().undo();
            REQUIRE(model == referenceModel);
            REQUIRE(referenceModel.removeRow(row, parent.toModelIndex(&referenceModel)));
            model.undoStack().redo();
            REQUIRE(model == referenceModel);
        }),
        NAMED_ROW("In project root", {0}, 0)
        NAMED_ROW("In notebook root", {1}, 0)
        NAMED_ROW("In chapter", {0, 0}, 0)
    )

    DATA_SECTION("move",
        TESTFUN([&] (ModelPath const& src, ModelPath const& dest) {
            { // Check undo
                QPersistentModelIndex srcParent = src.parentPath().toModelIndex(&model);
                int srcRow = src.leaf().first;
                QPersistentModelIndex destParent = dest.parentPath().toModelIndex(&model);
                int destRow = dest.leaf().first;

                REQUIRE(model.moveRow(srcParent, srcRow, destParent, destRow));
                model.undoStack().undo();
                REQUIRE(model == referenceModel);
            }
            { // Check redo
                QPersistentModelIndex srcParent = src.parentPath().toModelIndex(&referenceModel);
                int srcRow = src.leaf().first;
                QPersistentModelIndex destParent = dest.parentPath().toModelIndex(&referenceModel);
                int destRow = dest.leaf().first;
                REQUIRE(referenceModel.moveRow(srcParent, srcRow, destParent, destRow));
                model.undoStack().redo();
                REQUIRE(model == referenceModel);
            }
        }),
        NAMED_ROW("Same level (up)", {0, 0, 1}, {0, 0, 0})
        NAMED_ROW("Same level (down)", {0, 0, 0}, {0, 0, 2})
        NAMED_ROW("Up in hierarchy (from below)", {0, 0, 1, 0}, {0, 0, 1})
        NAMED_ROW("Up in hierarchy (from above)", {0, 0, 1, 0}, {0, 0, 2})
        NAMED_ROW("2x up in hierarchy (from below)", {0, 0, 1, 0}, {0, 0})
        NAMED_ROW("2x up in hierarchy (from above)", {0, 0, 1, 0}, {0, 1})
        NAMED_ROW("Down in hierarchy (from below)", {0, 1}, {0, 0, 1})
        NAMED_ROW("Down in hierarchy (from above)", {0, 0, 0}, {0, 0, 1, 0})
        NAMED_ROW("2x down in hierarchy (from below)", {0, 1}, {0, 0, 1, 2})
        NAMED_ROW("2x down in hierarchy (from above)", {0, 0}, {0, 1, 0, 0})
        NAMED_ROW("Scene into unrelated chapter", {0, 0, 0}, {0, 1, 0})
        NAMED_ROW("Scene to notebook", {0, 0, 0}, {1, 0})
        NAMED_ROW("Chapter to notebook", {0, 0, 1}, {1, 0})
    )

    DATA_SECTION("rename",
        TESTFUN([&model] (ModelPath const& path) {
            const char* testName = "foobar";
            QModelIndex idx = path.toModelIndex(&model);
            std::string oldName = model.nodeName(idx).toStdString();
            REQUIRE(model.setData(idx, testName, Qt::EditRole));
            REQUIRE(model.nodeName(idx).toStdString() == testName);
            model.undoStack().undo();
            REQUIRE(model.nodeName(idx).toStdString() == oldName);
            model.undoStack().redo();
            REQUIRE(model.nodeName(idx).toStdString() == testName);
        }),
        NAMED_ROW("Project root", {0})
        NAMED_ROW("Chapter", {0, 0})
        NAMED_ROW("Section", {0, 0, 0})
    )

    SECTION("properties modification")
    {
        ProjectProperties oldProperties = model.properties();
        ProjectProperties newProperties{"Awesome Title", "Superb Author", Language::en_US};
        model.setProperties(newProperties);
        model.undoStack().undo();
        REQUIRE(model.properties() == oldProperties);
        model.undoStack().redo();
        REQUIRE(model.properties() == newProperties);
    }
}

TEST_CASE("ProjectModel read/write", "[Model]")
{
    ProjectModel model{properties};
    fillModel(model);

    QString xml;
    REQUIRE(model.write(xml));

    ProjectModel loadedModel;
    REQUIRE(loadedModel.read(xml));

    REQUIRE(model == loadedModel);
}