/**********************************************************
 * @file   TextEditorParagraphNumbersSideBar.cpp
 * @author jan
 * @date   2/11/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtGui/QPainter>
#include "editor/sidebars/TextEditorParagraphNumbersSideBar.h"
#include "editor/TextEditor.h"

namespace novelist::editor {
    TextEditorParagraphNumbersSideBar::TextEditorParagraphNumbersSideBar(gsl::not_null<TextEditor*> editor) noexcept
            :TextEditorVerticalSideBar(editor)
    {
    }

    void TextEditorParagraphNumbersSideBar::paintEvent(QPaintEvent* event)
    {
        TextEditorVerticalSideBar::paintEvent(event);

        auto contentArea = editor()->contentArea();
        QPainter painter(this);
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
                QString number = QString::number(p.number() + 1);

                auto const y =   p.lines().at(0).baseline()
                               + contentArea.y()
                               - fontMetrics().ascent()
                               - editor()->scrollBarValues().second;

                painter.setPen(m_parNumberColor);
                painter.drawText(0, y, width() - 2, fontMetrics().height(), Qt::AlignRight, number);
            }
        }
    }

    int TextEditorParagraphNumbersSideBar::sideBarWidth() const noexcept
    {
        int digits = 1;
        int max = qMax(1ul, editor()->getDocument()->properties().countParagraphs());
        while (max >= 10) {
            max /= 10;
            ++digits;
        }

        int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits + 10;
        return space;
    }
}