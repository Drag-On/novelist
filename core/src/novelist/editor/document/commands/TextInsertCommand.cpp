/**********************************************************
 * @file   TextInsertCommand.cpp
 * @author jan
 * @date   3/18/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "editor/document/commands/TextInsertCommand.h"

namespace novelist::editor::internal {
    TextInsertCommand::TextInsertCommand(Document* doc, int pos, QString added) noexcept
            :m_doc(doc),
             m_pos(pos),
             m_added(std::move(added))
    {
        setText(Document::tr("text insertion in %1").arg(doc->properties().title()));
    }

    void TextInsertCommand::undo()
    {
        QTextCursor cursor(m_doc->m_doc.get());
        cursor.setPosition(m_pos + m_added.size());
        cursor.setPosition(m_pos, QTextCursor::KeepAnchor);

//            qDebug() << "Undo inserting" << cursor.selectedText() << "at" << m_pos << "(" << m_added.size() << ")";
//            QString infoText = cursor.block().text();
//            infoText.insert(m_pos, "‸");
//            infoText.insert(m_pos + m_added.size(), "‸");
//            qDebug() << "    " << infoText;

        cursor.removeSelectedText();
    }

    void TextInsertCommand::redo()
    {
        QTextCursor cursor(m_doc->m_doc.get());
        cursor.setPosition(m_pos);

//            qDebug() << "Redo inserting" << m_added << "at" << m_pos << "(" << m_added.size() << ")";
//            QString infoText = cursor.block().text();
//            infoText.insert(m_pos, "‸");
//            qDebug() << "    " << infoText;

        cursor.insertText(m_added);
    }

    int TextInsertCommand::id() const
    {
        return 0;
    }

    bool TextInsertCommand::mergeWith(const QUndoCommand* other)
    {
        auto cmd = dynamic_cast<TextInsertCommand const*>(other);
        if (!cmd)
            return false;
        if (m_pos + m_added.size() == cmd->m_pos && !isCompleteSentence(m_added, &m_doc->m_properties.language())) {
            m_added += cmd->m_added;
            return true;
        }
        return false;
    }

    int TextInsertCommand::undoPosition() const noexcept
    {
        return m_pos;
    }

    int TextInsertCommand::redoPosition() const noexcept
    {
        return m_pos + m_added.size();
    }
}