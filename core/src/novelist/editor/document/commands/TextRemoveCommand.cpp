/**********************************************************
 * @file   TextRemoveCommand.cpp
 * @author jan
 * @date   3/18/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "editor/document/commands/TextRemoveCommand.h"

namespace novelist::editor::internal {
    TextRemoveCommand::TextRemoveCommand(Document* doc, int pos, QString removed) noexcept
            :m_doc(doc),
             m_pos(pos),
             m_removed(std::move(removed))
    {
        setText(Document::tr("text removal in %1").arg(doc->properties().title()));
    }

    void TextRemoveCommand::undo()
    {
        QTextCursor cursor(m_doc->m_doc.get());
        cursor.setPosition(m_pos);

//            qDebug() << "Undo removing" << m_removed << "at" << m_pos << "(" << m_removed.size() << ")";
//            QString infoText = cursor.block().text();
//            infoText.insert(m_pos, "‸");
//            qDebug() << "    " << infoText;

        cursor.insertText(m_removed);
    }

    void TextRemoveCommand::redo()
    {
        QTextCursor cursor(m_doc->m_doc.get());
        cursor.setPosition(m_pos + m_removed.size());
        cursor.setPosition(m_pos, QTextCursor::KeepAnchor);

//            qDebug() << "Redo removing" << m_removed << "at" << m_pos << "(" << m_removed.size() << ")";
//            QString infoText = cursor.block().text();
//            infoText.insert(m_pos, "‸");
//            infoText.insert(m_pos + m_removed.size(), "‸");
//            qDebug() << "    " << infoText;

        auto charFormat = cursor.charFormat();

        cursor.removeSelectedText();

        // If this leaves the block empty make sure the block char format doesn't jump to something set previously,
        // but instead retain the last value
        if (cursor.atBlockStart() && cursor.atBlockEnd()) {
            cursor.setBlockCharFormat(charFormat);
            // Workaround. See TextCursor::setCharacterFormat().
            cursor.insertText(" ");
            cursor.deletePreviousChar();
        }
    }

    int TextRemoveCommand::id() const
    {
        return 1;
    }

    bool TextRemoveCommand::mergeWith(QUndoCommand const* other)
    {
        auto cmd = dynamic_cast<TextRemoveCommand const*>(other);
        if (!cmd)
            return false;
        if (cmd->m_pos == m_pos && !isCompleteSentence(m_removed, &m_doc->m_properties.language())) {
            m_removed += cmd->m_removed;
            return true;
        }
        if (cmd->m_pos + cmd->m_removed.size() == m_pos
                && !isCompleteSentence(cmd->m_removed, &m_doc->m_properties.language())) {
            m_pos = cmd->m_pos;
            m_removed = cmd->m_removed + m_removed;
            return true;
        }
        return false;
    }

    int TextRemoveCommand::undoPosition() const noexcept
    {
        return m_pos + m_removed.size();
    }

    int TextRemoveCommand::redoPosition() const noexcept
    {
        return m_pos;
    }
}