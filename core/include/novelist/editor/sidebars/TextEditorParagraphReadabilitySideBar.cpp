/**********************************************************
 * @file   TextEditorParagraphReadabilitySideBar.cpp
 * @author jan
 * @date   2/23/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "TextEditorParagraphReadabilitySideBar.h"

namespace novelist::editor {
    QColor TextEditorParagraphReadabilitySideBar::colorFor(TextParagraph const& par) const noexcept
    {
        return QColor(0, 0, 0);
    }
}