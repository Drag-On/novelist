/**********************************************************
 * @file   BlockInsertCommand.cpp
 * @author jan
 * @date   3/18/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "editor/document/commands/BlockInsertCommand.h"

namespace novelist::editor::internal {
    BlockInsertCommand::BlockInsertCommand(Document* doc, int pos) noexcept
            :m_doc(doc),
             m_pos(pos)
    {
        setText(Document::tr("new paragraph in %1").arg(doc->properties().title()));
    }

    void BlockInsertCommand::undo()
    {
        QTextCursor cursor(m_doc->m_doc.get());
        cursor.setPosition(m_pos);
        cursor.deleteChar();
    }

    void BlockInsertCommand::redo()
    {
        QTextCursor cursor(m_doc->m_doc.get());
        cursor.setPosition(m_pos);
        auto prevCharFormat = cursor.charFormat();
        cursor.insertBlock();
        cursor.setBlockCharFormat(prevCharFormat);
    }

    int BlockInsertCommand::undoPosition() const noexcept
    {
        return m_pos;
    }

    int BlockInsertCommand::redoPosition() const noexcept
    {
        return m_pos + 1;
    }
}