/**********************************************************
 * @file   TextEditorSideBar.cpp
 * @author jan
 * @date   2/11/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include "editor/sidebars/TextEditorSideBar.h"
#include "editor/TextEditor.h"
#include <QPainter>

namespace novelist::editor {
    TextEditorSideBar::TextEditorSideBar(gsl::not_null<TextEditor*> editor) noexcept
            :QWidget(editor),
             m_editor(editor)
    {
    }

    TextEditor const* TextEditorSideBar::editor() const noexcept
    {
        return m_editor;
    }

    void TextEditorSideBar::paintEvent(QPaintEvent* event)
    {
        QWidget::paintEvent(event);

        QPainter painter(this);
        painter.fillRect(event->rect(), m_AreaColor);
    }

    TextEditorVerticalSideBar::TextEditorVerticalSideBar(gsl::not_null<TextEditor*> editor) noexcept
            :TextEditorSideBar(editor)
    {
    }

    QSize TextEditorVerticalSideBar::sizeHint() const
    {
        return QSize(sideBarWidth(), 0);
    }

    TextEditorSideBar::UpdateTriggers TextEditorVerticalSideBar::updateTriggers() const noexcept
    {
        return {UpdateTrigger::Resize, UpdateTrigger::LineCountChange, UpdateTrigger::VerticalScroll};
    }

    TextEditorHorizontalSideBar::TextEditorHorizontalSideBar(gsl::not_null<TextEditor*> editor) noexcept
            :TextEditorSideBar(editor)
    {
    }

    QSize TextEditorHorizontalSideBar::sizeHint() const
    {
        return QSize(0, sideBarHeight());
    }

    TextEditorSideBar::UpdateTriggers TextEditorHorizontalSideBar::updateTriggers() const noexcept
    {
        return {UpdateTrigger::Resize, UpdateTrigger::HorizontalScroll};
    }
}