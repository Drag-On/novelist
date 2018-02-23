/**********************************************************
 * @file   TextEditorParagraphColorSideBar.h
 * @author jan
 * @date   2/23/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TEXTEDITORPARAGRAPHCOLORSIDEBAR_H
#define NOVELIST_TEXTEDITORPARAGRAPHCOLORSIDEBAR_H

#include "TextEditorSideBar.h"

namespace novelist::editor {
    class TextEditorParagraphColorSideBar : public TextEditorVerticalSideBar {
    Q_OBJECT

    public:
        explicit TextEditorParagraphColorSideBar(gsl::not_null<TextEditor*> editor) noexcept;

        int sideBarWidth() const noexcept override;

    protected:
        void paintEvent(QPaintEvent* event) override;

        virtual QColor colorFor(TextParagraph const& par) const noexcept = 0;
    };
}

#endif //NOVELIST_TEXTEDITORPARAGRAPHCOLORSIDEBAR_H
