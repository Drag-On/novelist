/**********************************************************
 * @file   TextEditorLineNumberSideBar.h
 * @author jan
 * @date   2/17/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TEXTEDITORLINENUMBERSIDEBAR_H
#define NOVELIST_TEXTEDITORLINENUMBERSIDEBAR_H

#include "TextEditorSideBar.h"

namespace novelist::editor {
    class TextEditorLineNumbersSideBar : public TextEditorVerticalSideBar {
    Q_OBJECT

    public:
        TextEditorLineNumbersSideBar(gsl::not_null<TextEditor*> editor) noexcept;

        int sideBarWidth() const noexcept override;

    protected:
        void paintEvent(QPaintEvent* event) override;

    private:
        QColor const m_lineNumberAreaColor = QColor(250, 250, 250);
        QColor const m_lineNumberColor = QColor(130, 130, 130);
        QFont const m_lineNumberFont;
    };
}

#endif //NOVELIST_TEXTEDITORLINENUMBERSIDEBAR_H
