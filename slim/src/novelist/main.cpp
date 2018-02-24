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
    mgr.push_back(TextFormatData{"Paragraph"});
    mgr.push_back(TextFormatData{"Dream", Alignment::Left, Margin{}, Indentation{}, CharacterFormat{false, true, false, false, false, false}});
    mgr.push_back(TextFormatData{"Poem", Alignment::Left, Margin{}, Indentation{1, 1, false}, CharacterFormat{}});
    auto textEditor = new TextEditor;
    textEditor->setDocument(std::make_unique<Document>(&mgr, "Test", getProjectLanguage(Language::English, Country::UnitedStates)));

    QUndoView undoView(&textEditor->getDocument()->undoStack());
    undoView.show();


    QMainWindow wnd;
    QAction* makeItalic = new QAction("Make italic", &wnd);
    QObject::connect(makeItalic, &QAction::triggered, [textEditor]{ qDebug() << "makeItalic"; textEditor->getCursor().setCharacterFormat(1);});
    TextEditorToolBar toolBar(&wnd);
    toolBar.setEditor(textEditor);
    toolBar.addAction(makeItalic);
    wnd.setCentralWidget(textEditor);
    wnd.addToolBar(&toolBar);
    wnd.show();

    app.exec();

    return 0;
}
