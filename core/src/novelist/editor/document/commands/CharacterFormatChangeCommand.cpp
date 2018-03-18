/**********************************************************
 * @file   CharacterFormatChangeCommand.cpp
 * @author jan
 * @date   3/18/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "editor/document/commands/CharacterFormatChangeCommand.h"

namespace novelist::editor::internal {
    CharacterFormatChangeCommand::CharacterFormatChangeCommand(Document* doc, int pos, int length,
                                                               TextFormat::WeakId id) noexcept
            :m_doc(doc),
             m_pos(pos),
             m_length(length),
             m_formatId(id)
    {
        setText(Document::tr("change character format in %1").arg(doc->properties().title()));
    }

    void CharacterFormatChangeCommand::undo()
    {
        // Note: m_formatId toggles between the old and new format, e.g. after redo() it will contain the format
        //       for undo. The algorithm stays the same, so we just forward the call here.
        redo();
    }

    void CharacterFormatChangeCommand::redo()
    {
        QTextCursor cursor(m_doc->m_doc.get());
        cursor.setPosition(m_pos + m_length);
        cursor.setPosition(m_pos, QTextCursor::KeepAnchor);

        auto format = *m_doc->m_formatMgr->getTextCharFormat(m_formatId);
        if (!cursor.hasSelection() && cursor.atBlockStart() && cursor.atBlockEnd()) {
            // Empty block, set block char format
            auto curFormatId = m_doc->m_formatMgr->getIdOfCharFormat(cursor.blockCharFormat());
            cursor.setBlockCharFormat(format);
            m_formatId = curFormatId;
            // Note: The following works around a display bug where changing the block char format makes the whole
            //       editor disappear:
            //       https://bugreports.qt.io/browse/QTBUG-40512
            // TODO: Remove this once above ticket has been resolved.
            cursor.insertText(" ");
            cursor.deletePreviousChar();
        }
        else {
            auto curFormatId = m_doc->m_formatMgr->getIdOfCharFormat(cursor.charFormat());
            cursor.setCharFormat(format);
            m_formatId = curFormatId;
        }
    }

    int CharacterFormatChangeCommand::undoPosition() const noexcept
    {
        return m_pos;
    }

    int CharacterFormatChangeCommand::redoPosition() const noexcept
    {
        return m_pos;
    }
}