/**********************************************************
 * @file   TextEditorParagraphReadabilitySideBar.h
 * @author jan
 * @date   2/23/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TEXTEDITORPARAGRAPHREADABILITYSIDEBAR_H
#define NOVELIST_TEXTEDITORPARAGRAPHREADABILITYSIDEBAR_H

#include "TextEditorParagraphColorSideBar.h"

namespace novelist::editor {
    class TextEditorParagraphReadabilitySideBar : public TextEditorParagraphColorSideBar {
        Q_OBJECT

    public:
        using TextEditorParagraphColorSideBar::TextEditorParagraphColorSideBar;

    protected:
        QColor colorFor(TextParagraph const& par) const noexcept override;
    };
}

#endif //NOVELIST_TEXTEDITORPARAGRAPHREADABILITYSIDEBAR_H
