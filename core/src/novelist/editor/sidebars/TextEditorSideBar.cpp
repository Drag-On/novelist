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
        triggers.set(UpdateTrigger::LineCountChange);
        triggers.set(UpdateTrigger::VerticalScroll);
        return triggers;
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
        TextEditorSideBar::UpdateTriggers triggers;
        triggers.set(UpdateTrigger::Resize);
        triggers.set(UpdateTrigger::HorizontalScroll);
        return triggers;
    }
}