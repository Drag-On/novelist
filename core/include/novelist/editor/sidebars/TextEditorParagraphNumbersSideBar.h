/**********************************************************
 * @file   TextEditorParagraphNumbersSideBar.h
 * @author jan
 * @date   2/11/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TEXTEDITORPARAGRAPHNUMBERSSIDEBAR_H
#define NOVELIST_TEXTEDITORPARAGRAPHNUMBERSSIDEBAR_H

#include "TextEditorSideBar.h"

namespace novelist::editor {
    class TextEditorParagraphNumbersSideBar : public TextEditorVerticalSideBar {
        Q_OBJECT

    public:
        TextEditorParagraphNumbersSideBar(gsl::not_null<TextEditor*> editor) noexcept;

        int sideBarWidth() const noexcept override;

    protected:
        void paintEvent(QPaintEvent* event) override;

    private:
        QColor const m_parNumberColor = QColor(130, 130, 130);
        QFont const m_parNumberFont;
    };
}

#endif //NOVELIST_TEXTEDITORPARAGRAPHNUMBERSSIDEBAR_H
