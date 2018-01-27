/**********************************************************
 * @file   Document.cpp
 * @author jan
 * @date   1/25/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtGui/QTextBlock>
#include "editor/document/Document.h"

namespace novelist::editor {
    Document::Document(gsl::not_null<TextFormatManager*> formatMgr, QString title,
            gsl::not_null<ProjectLanguage*> lang) noexcept
            :m_properties(this, std::move(title), lang),
             m_formatMgr(formatMgr),
             m_doc(std::make_unique<QTextDocument>())
    {
    }

    QUndoStack& Document::undoStack() noexcept
    {
        return m_undoStack;
    }

    Properties& Document::properties() noexcept
    {
        return m_properties;
    }

    Properties const& Document::properties() const noexcept
    {
        return m_properties;
    }

    QString Document::toRawText() const noexcept
    {
        return m_doc->toRawText();
    }

    bool Document::empty() const noexcept
    {
        return m_doc->isEmpty();
    }

    void Document::onParagraphFormatChanged(int blockIdx) noexcept
    {
        // Auto text indent logic
        if (blockIdx == 0 || blockIdx >= m_doc->blockCount())
            return;

        auto block = m_doc->findBlockByNumber(blockIdx);
        auto blockFormat = block.blockFormat();
        auto formatId = m_formatMgr->getIdOfBlockFormat(blockFormat);
        auto prevFormatId = m_formatMgr->getIdOfBlockFormat(m_doc->findBlockByNumber(blockIdx - 1).blockFormat());

        // If the paragraph doesn't have auto text indentation enabled there is nothing to do
        if (!m_formatMgr->getTextFormat(m_formatMgr->indexFromId(formatId))->m_data.m_indentation.m_autoTextIndent)
            return;

        bool textIndentNeeded = needAutoTextIndent(formatId, prevFormatId);
        if (textIndentNeeded && blockFormat.textIndent() != blockFormat.indent() + m_doc->indentWidth()) {
            blockFormat.setTextIndent(blockFormat.indent() + m_doc->indentWidth());
            QTextCursor cursor(block);
            cursor.joinPreviousEditBlock();
            cursor.setBlockFormat(blockFormat);
            cursor.endEditBlock();
            onParagraphFormatChanged(blockIdx + 1);
        }
        else if (!textIndentNeeded && blockFormat.textIndent() != blockFormat.indent()) {
            blockFormat.setTextIndent(blockFormat.indent());
            QTextCursor cursor(block);
            cursor.joinPreviousEditBlock();
            cursor.setBlockFormat(blockFormat);
            cursor.endEditBlock();
            onParagraphFormatChanged(blockIdx - 1);
        }
    }

    bool Document::needAutoTextIndent(TextFormat::WeakId thisParFormat, TextFormat::WeakId prevParFormat) const noexcept
    {
        return thisParFormat == prevParFormat;
    }
}