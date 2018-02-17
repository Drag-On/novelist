/**********************************************************
 * @file   TextEditorSideBar.cpp
 * @author jan
 * @date   2/11/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include "editor/TextEditorSideBar.h"
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
        TextEditorSideBar::UpdateTriggers triggers;
        triggers.set(UpdateTrigger::Resize);
        triggers.set(UpdateTrigger::ParagraphCountChange);
        triggers.set(UpdateTrigger::VerticalScroll);
        return triggers;
    }

    TestSideBar::TestSideBar(gsl::not_null<TextEditor*> editor, QColor clr)
            : TextEditorVerticalSideBar(editor),
              m_color(clr)
    {
    }

    int TestSideBar::sideBarWidth() const noexcept
    {
        return 50;
    }

    void TestSideBar::paintEvent(QPaintEvent* event)
    {
        QWidget::paintEvent(event);

        QPainter painter(this);
        painter.fillRect(event->rect(), m_color);

        painter.drawText(10, 10, "Hello");
    }

    TestSideBar::UpdateTriggers TestSideBar::updateTriggers() const noexcept
    {
        return UpdateTriggers{};
    }
}