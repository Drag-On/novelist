/**********************************************************
 * @file   main.cpp
 * @author jan
 * @date   2/3/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <QtWidgets/QApplication>
#include <QtWidgets/QUndoView>
#include <editor/TextEditor.h>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationDisplayName("Novelist Slim");
    QApplication::setApplicationName("Novelist Slim");
    QApplication::setOrganizationName("Novelist-Org");
    QApplication::setOrganizationDomain("novelist-nodomain.org");

#ifdef _WIN32
    QIcon::setThemeSearchPaths(QStringList(":/icons"));
    QIcon::setThemeName("tango");
#endif

    using namespace novelist::editor;
    TextFormatManager mgr;
    mgr.push_back(TextFormatData());
    TextEditor textEditor;
    textEditor.setDocument(std::make_unique<Document>(&mgr, "Test", getProjectLanguage(Language::English, Country::UnitedStates)));
    textEditor.addAction(textEditor.editorActions().m_undoAction);
    textEditor.addAction(textEditor.editorActions().m_redoAction);
    textEditor.show();

    QUndoView undoView(&textEditor.getDocument()->undoStack());
    undoView.show();

    app.exec();

    return 0;
}
