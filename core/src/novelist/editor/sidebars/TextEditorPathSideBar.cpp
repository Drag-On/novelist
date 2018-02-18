/**********************************************************
 * @file   TextEditorPathSideBar.cpp
 * @author jan
 * @date   2/18/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtGui/QPainter>
#include "editor/sidebars/TextEditorPathSideBar.h"

namespace novelist::editor {
    TextEditorPathSideBar::TextEditorPathSideBar(gsl::not_null<TextEditor*> editor, QStringList path) noexcept
            :TextEditorHorizontalSideBar(editor),
             m_path(std::move(path))
    {
    }

    int TextEditorPathSideBar::sideBarHeight() const noexcept
    {
        return QFontMetrics(m_font).height() + 8;
    }

    void TextEditorPathSideBar::paintEvent(QPaintEvent* event)
    {
        QPainter painter(this);
        painter.fillRect(event->rect(), m_areaColor);

        QFontMetrics fm(m_font);
        auto const sepWidth = fm.width(m_separator);
        painter.setFont(m_font);
        painter.setPen(m_fontColor);
        int x = 2;
        auto iter = m_path.begin();
        if (iter != m_path.end()) {
            auto const& s = *iter;
            auto const w = fm.width(s);
            painter.drawText(x, 2, w, fm.height(), Qt::AlignLeft, s);
            x += w;
        }

        for (++iter; iter != m_path.end(); ++iter) {
            painter.setPen(m_separatorColor);
            painter.drawText(x, 0, sepWidth, fm.height() + 4, Qt::AlignLeft, m_separator);
            x += sepWidth;

            painter.setPen(m_fontColor);
            auto const& s = *iter;
            auto const w = fm.width(s);
            painter.drawText(x, 2, w, fm.height(), Qt::AlignLeft, s);
            x += w;
        }

    }
}