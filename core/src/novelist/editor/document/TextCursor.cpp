/**********************************************************
 * @file   TextCursor.cpp
 * @author jan
 * @date   1/27/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "util/Overloaded.h"
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
            :m_doc(doc),
             m_cursor(QTextCursor(doc->m_doc.get()))
    {
        m_cursor.setPosition(anchor);
        m_cursor.setPosition(pos, QTextCursor::MoveMode::KeepAnchor);
    }

    int TextCursor::position() const noexcept
    {
        return m_cursor.position();
    }

    int TextCursor::anchor() const noexcept
    {
        return m_cursor.anchor();
    }

    void TextCursor::setPosition(int pos) noexcept
    {
        if (pos <= 0)
            m_cursor.movePosition(QTextCursor::MoveOperation::Start);
        else if (pos >= m_doc->m_doc->characterCount())
            m_cursor.movePosition(QTextCursor::MoveOperation::End);
        else
            m_cursor.setPosition(pos);
    }

    void TextCursor::select(int pos, int anchor) noexcept
    {
        if (anchor <= 0)
            m_cursor.movePosition(QTextCursor::MoveOperation::Start);
        else if (anchor >= m_doc->m_doc->characterCount())
            m_cursor.movePosition(QTextCursor::MoveOperation::End);
        else
            m_cursor.setPosition(anchor);
        if (pos <= 0)
            m_cursor.movePosition(QTextCursor::MoveOperation::Start, QTextCursor::KeepAnchor);
        else if (pos >= m_doc->m_doc->characterCount())
            m_cursor.movePosition(QTextCursor::MoveOperation::End, QTextCursor::KeepAnchor);
        else
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

    QString TextCursor::selectedText() const noexcept
    {
        return m_cursor.selectedText();
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
        if (hasSelection()) {
            deleteSelected();
            return;
        }
        if (atStart())
            return;
        m_cursor.movePosition(QTextCursor::MoveOperation::PreviousCharacter, QTextCursor::KeepAnchor);
        m_doc->undoStack().push(new internal::TextRemoveCommand(m_doc, m_cursor.selectionStart(), m_cursor.selectedText()));
    }

    void TextCursor::deleteNext() noexcept
    {
        if (hasSelection()) {
            deleteSelected();
            return;
        }
        if (atEnd())
            return;
        m_cursor.movePosition(QTextCursor::MoveOperation::NextCharacter, QTextCursor::KeepAnchor);
        m_doc->undoStack().push(new internal::TextRemoveCommand(m_doc, m_cursor.selectionStart(), m_cursor.selectedText()));
    }

    void TextCursor::deleteSelected() noexcept
    {
        if (!hasSelection())
            return;
        m_doc->undoStack().push(new internal::TextRemoveCommand(m_doc, m_cursor.selectionStart(), m_cursor.selectedText()));
    }

    void TextCursor::insertParagraph() noexcept
    {
        if (hasSelection())
            deleteSelected();
        m_doc->undoStack().push(new internal::BlockInsertCommand(m_doc, position()));
    }

    void TextCursor::insertText(QString text) noexcept
    {
        if (hasSelection()) {
            m_doc->undoStack().beginMacro(Document::tr("replacing text"));
            auto endMacro = gsl::finally([this]{m_doc->undoStack().endMacro();});
            deleteSelected();
            m_doc->undoStack().push(new internal::TextInsertCommand(m_doc, m_cursor.selectionStart(), std::move(text)));
        }
        else
            m_doc->undoStack().push(new internal::TextInsertCommand(m_doc, m_cursor.position(), std::move(text)));
    }

    TextFormat::WeakId TextCursor::paragraphFormat() const noexcept
    {
        return m_doc->m_formatMgr->getIdOfBlockFormat(m_cursor.blockFormat());
    }

    void TextCursor::setParagraphFormat(TextFormat::WeakId id) noexcept
    {
        m_cursor.setBlockFormat(*m_doc->m_formatMgr->getTextBlockFormat(id));
        m_doc->onParagraphFormatChanged(m_cursor.blockNumber());
        // TODO: UndoRedo
    }

    TextFormat::WeakId TextCursor::characterFormat() const noexcept
    {
        return m_doc->m_formatMgr->getIdOfCharFormat(m_cursor.charFormat());
    }

    void TextCursor::setCharacterFormat(TextFormat::WeakId id) noexcept
    {
        auto format = *m_doc->m_formatMgr->getTextCharFormat(id);
        m_cursor.setCharFormat(format);
        m_cursor.setBlockCharFormat(format);
        // TODO: UndoRedo
    }
}