/**********************************************************
 * @file   TextCursor.cpp
 * @author jan
 * @date   1/27/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "editor/document/TextCursor.h"
#include "editor/document/Document.h"

namespace novelist::editor {
    TextCursor::TextCursor(gsl::not_null<Document*> doc) noexcept
    :TextCursor(doc, 0)
    {
    }

    TextCursor::TextCursor(gsl::not_null<Document*> doc, int pos) noexcept
    :TextCursor(doc, pos, pos)
    {
    }

    TextCursor::TextCursor(gsl::not_null<Document*> doc, int pos, int anchor) noexcept
    : m_doc(doc),
      m_cursor(QTextCursor(doc->m_doc.get()))
    {
        m_cursor.setPosition(anchor);
        m_cursor.setPosition(pos, QTextCursor::MoveMode::KeepAnchor);
    }

    int TextCursor::position() const noexcept
    {
        return m_cursor.position();
    }

    void TextCursor::setPosition(int pos) noexcept
    {
        m_cursor.setPosition(pos);
    }

    void TextCursor::select(int pos, int anchor) noexcept
    {
        m_cursor.setPosition(anchor);
        m_cursor.setPosition(pos, QTextCursor::MoveMode::KeepAnchor);
    }

    std::pair<int, int> TextCursor::getSelection() const noexcept
    {
        return std::pair<int, int>(m_cursor.position(), m_cursor.anchor());
    }

    bool TextCursor::hasSelection() const noexcept
    {
        return m_cursor.hasSelection();
    }

    bool TextCursor::atParagraphStart() const noexcept
    {
        return m_cursor.atBlockStart();
    }

    bool TextCursor::atParagraphEnd() const noexcept
    {
        return m_cursor.atBlockEnd();
    }

    bool TextCursor::atStart() const noexcept
    {
        return m_cursor.atStart();
    }

    bool TextCursor::atEnd() const noexcept
    {
        return m_cursor.atEnd();
    }

    void TextCursor::deletePrevious() noexcept
    {
        m_cursor.deletePreviousChar();
    }

    void TextCursor::deleteNext() noexcept
    {
        m_cursor.deleteChar();
    }

    void TextCursor::insertParagraph() noexcept
    {
        m_cursor.insertBlock();
    }

    void TextCursor::insertText(QString text) noexcept
    {
        m_cursor.insertText(text);
    }

    TextFormat::WeakId TextCursor::paragraphFormat() const noexcept
    {
        return m_doc->m_formatMgr->getIdOfBlockFormat(m_cursor.blockFormat());
    }

    void TextCursor::setParagraphFormat(TextFormat::WeakId id) noexcept
    {
        m_cursor.setBlockFormat(*m_doc->m_formatMgr->getTextBlockFormat(id));
        m_doc->onParagraphFormatChanged(m_cursor.blockNumber());
    }

    TextFormat::WeakId TextCursor::characterFormat() const noexcept
    {
        return m_doc->m_formatMgr->getIdOfCharFormat(m_cursor.charFormat());
    }

    void TextCursor::setCharacterFormat(TextFormat::WeakId id) noexcept
    {
        m_cursor.setCharFormat(*m_doc->m_formatMgr->getTextCharFormat(id));
    }
}