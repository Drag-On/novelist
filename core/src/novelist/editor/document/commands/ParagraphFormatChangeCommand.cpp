/**********************************************************
 * @file   ParagraphFormatChangeCommand.cpp
 * @author jan
 * @date   3/18/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "editor/document/commands/ParagraphFormatChangeCommand.h"

namespace novelist::editor::internal {
    ParagraphFormatChangeCommand::ParagraphFormatChangeCommand(Document* doc, int blockNo,
                                                               TextFormat::WeakId id) noexcept
    : m_doc(doc),
      m_blockNo(blockNo),
      m_formatId(id)
    {
        setText(Document::tr("change paragraph format in %1").arg(doc->properties().title()));
    }

    void ParagraphFormatChangeCommand::undo()
    {
        // Note: m_formatId toggles between the old and new format, e.g. after redo() it will contain the format
        //       for undo. The algorithm stays the same, so we just forward the call here.
        redo();
    }

    void ParagraphFormatChangeCommand::redo()
    {
        QTextCursor cursor(m_doc->m_doc.get());
        cursor.setPosition(m_doc->m_doc->findBlockByNumber(m_blockNo).position());

        auto curFormatId = m_doc->m_formatMgr->getIdOfBlockFormat(cursor.blockFormat());
        cursor.setBlockFormat(*m_doc->m_formatMgr->getTextBlockFormat(m_formatId));
        m_doc->onParagraphFormatChanged(m_blockNo);
        m_formatId = curFormatId;
    }

    int ParagraphFormatChangeCommand::undoPosition() const noexcept
    {
        return m_doc->m_doc->findBlockByNumber(m_blockNo).position();
    }

    int ParagraphFormatChangeCommand::redoPosition() const noexcept
    {
        return m_doc->m_doc->findBlockByNumber(m_blockNo).position();
    }
}