/**********************************************************
 * @file   TextEditorParagraphColorSideBar.cpp
 * @author jan
 * @date   2/23/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtGui/QPainter>
#include "editor/TextEditor.h"
#include "editor/sidebars/TextEditorParagraphColorSideBar.h"

namespace novelist::editor {
    TextEditorParagraphColorSideBar::TextEditorParagraphColorSideBar(gsl::not_null<TextEditor*> editor) noexcept
            :TextEditorVerticalSideBar(editor)
    {
    }

    int TextEditorParagraphColorSideBar::sideBarWidth() const noexcept
    {
        return 15;
    }

    void TextEditorParagraphColorSideBar::paintEvent(QPaintEvent* event)
    {
        TextEditorVerticalSideBar::paintEvent(event);

        auto contentArea = editor()->contentArea();
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        bool foundVisible = false;
        for (auto const& p : *editor()->getDocument()) {
            if (editor()->isParagraphVisible(p))
                foundVisible = true;
            else if (foundVisible)
                break; // If there have been visible blocks before we can end
            else continue; // Not visible but not found a visible one yet, so look further down

            auto bb = p.boundingRect();
            bb.translate(contentArea.x(), contentArea.y() - editor()->scrollBarValues().second);

            if (bb.top() <= event->rect().bottom() && bb.bottom() >= event->rect().top()) {

                auto const y = bb.top() + 3;
                auto const h = bb.bottom() - y - 3;
                auto const color = colorFor(p);

                QPainterPath path;
                path.addRoundedRect(QRectF(2, y, 11, h), 5, 5);
                QPen pen(color);
                painter.setPen(pen);
                painter.fillPath(path, color);
//                painter.drawPath(path);
            }
        }
    }
}