/**********************************************************
 * @file   TextEditorLineNumberSideBar.cpp
 * @author jan
 * @date   2/17/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "editor/sidebars/TextEditorLineNumbersSideBar.h"
#include "editor/TextEditor.h"
#include <QtGui/QPainter>

namespace novelist::editor {
    TextEditorLineNumbersSideBar::TextEditorLineNumbersSideBar(gsl::not_null<TextEditor*> editor) noexcept
            :TextEditorVerticalSideBar(editor)
    {
    }

    void TextEditorLineNumbersSideBar::paintEvent(QPaintEvent* event)
    {
        auto contentArea = editor()->contentArea();

        QPainter painter(this);
        painter.fillRect(event->rect(), m_lineNumberAreaColor);

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

                for (size_t i = 0; i < p.lines().size(); ++i) {
                    QString number = QString::number(p.firstLineNo() + i + 1);

                    auto const y =   p.lines().at(i).baseline()
                                   + contentArea.y()
                                   - fontMetrics().ascent()
                                   - editor()->scrollBarValues().second;

                    painter.setPen(m_lineNumberColor);
                    painter.drawText(0, y, width() - 2, fontMetrics().height(), Qt::AlignRight, number);
                }
            }
        }
    }

    int TextEditorLineNumbersSideBar::sideBarWidth() const noexcept
    {
        int digits = 1;
        int max = qMax(1ul, editor()->getDocument()->properties().countLines());
        while (max >= 10) {
            max /= 10;
            ++digits;
        }

        int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits + 10;
        return space;
    }
}