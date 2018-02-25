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
#include <QDebug>
#include <editor/TextEditor.h>
#include <QtGui/QWindow>
#include <QtWidgets/QMainWindow>
#include <editor/TextEditorToolBar.h>

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
    mgr.push_back(TextFormatData{"Paragraph", QIcon{":/icons/format-custom-letters"}});
    mgr.push_back(TextFormatData{"Thoughts", QIcon{":/icons/format-custom-thoughtbubble"}, Alignment::Left, Margin{}, Indentation{}, CharacterFormat{false, true, false, false, false, false}});
    mgr.push_back(TextFormatData{"Poetry", QIcon{":/icons/format-custom-feather"}, Alignment::Left, Margin{}, Indentation{1, 1, false}, CharacterFormat{}});
    auto textEditor = new TextEditor;
    textEditor->setDocument(std::make_unique<Document>(&mgr, "Test", getProjectLanguage(Language::English, Country::UnitedStates)));

    QUndoView undoView(&textEditor->getDocument()->undoStack());
    undoView.show();

    QMainWindow wnd;
    TextEditorToolBar toolBar(&wnd);
    toolBar.setEditor(textEditor);
    wnd.setCentralWidget(textEditor);
    wnd.addToolBar(&toolBar);
    wnd.show();

    app.exec();

    return 0;
}
