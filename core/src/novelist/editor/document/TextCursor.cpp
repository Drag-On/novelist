/**********************************************************
 * @file   TextCursor.cpp
 * @author jan
 * @date   1/27/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QTextFragment>
#include "editor/document/TextCursor.h"
#include "editor/document/Document.h"
#include "editor/document/commands/TextInsertCommand.h"
#include "editor/document/commands/TextRemoveCommand.h"
#include "editor/document/commands/BlockInsertCommand.h"
#include "editor/document/commands/ParagraphFormatChangeCommand.h"
#include "editor/document/commands/CharacterFormatChangeCommand.h"

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

    void TextCursor::move(TextCursor::MoveOperation op) noexcept
    {
        switch (op) {
            case MoveOperation::Up:
                m_cursor.movePosition(QTextCursor::MoveOperation::Up);
                break;
            case MoveOperation::Down:
                m_cursor.movePosition(QTextCursor::MoveOperation::Down);
                break;
            case MoveOperation::Left:
                m_cursor.movePosition(QTextCursor::MoveOperation::PreviousCharacter);
                break;
            case MoveOperation::Right:
                m_cursor.movePosition(QTextCursor::MoveOperation::NextCharacter);
                break;
            case MoveOperation::Start:
                m_cursor.movePosition(QTextCursor::MoveOperation::Start);
                break;
            case MoveOperation::End:
                m_cursor.movePosition(QTextCursor::MoveOperation::End);
                break;
            case MoveOperation::StartOfLine:
                m_cursor.movePosition(QTextCursor::MoveOperation::StartOfLine);
                break;
            case MoveOperation::EndOfLine:
                m_cursor.movePosition(QTextCursor::MoveOperation::EndOfLine);
                break;
            case MoveOperation::StartOfWord:
                m_cursor.movePosition(QTextCursor::MoveOperation::StartOfWord);
                break;
            case MoveOperation::EndOfWord:
                m_cursor.movePosition(QTextCursor::MoveOperation::EndOfWord);
                break;
            case MoveOperation::StartOfParagraph:
                m_cursor.movePosition(QTextCursor::MoveOperation::StartOfBlock);
                break;
            case MoveOperation::EndOfParagraph:
                m_cursor.movePosition(QTextCursor::MoveOperation::EndOfBlock);
                break;
            case MoveOperation::StartOfNextLine:
                m_cursor.movePosition(QTextCursor::MoveOperation::StartOfLine);
                m_cursor.movePosition(QTextCursor::MoveOperation::Down);
                break;
            case MoveOperation::StartOfPreviousLine:
                m_cursor.movePosition(QTextCursor::MoveOperation::StartOfLine);
                m_cursor.movePosition(QTextCursor::MoveOperation::Up);
                break;
            case MoveOperation::StartOfNextWord:
                m_cursor.movePosition(QTextCursor::MoveOperation::NextWord);
                break;
            case MoveOperation::StartOfPreviousWord:
                m_cursor.movePosition(QTextCursor::MoveOperation::PreviousWord);
                break;
            case MoveOperation::StartOfNextParagraph:
                m_cursor.movePosition(QTextCursor::MoveOperation::NextBlock);
                break;
            case MoveOperation::StartOfPreviousParagraph:
                m_cursor.movePosition(QTextCursor::MoveOperation::PreviousBlock);
                break;
            default:
                break;
        }
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

    void TextCursor::select(TextCursor::MoveOperation op) noexcept
    {
        switch (op) {
            case MoveOperation::Up:
                m_cursor.movePosition(QTextCursor::MoveOperation::Up, QTextCursor::KeepAnchor);
                break;
            case MoveOperation::Down:
                m_cursor.movePosition(QTextCursor::MoveOperation::Down, QTextCursor::KeepAnchor);
                break;
            case MoveOperation::Left:
                m_cursor.movePosition(QTextCursor::MoveOperation::PreviousCharacter, QTextCursor::KeepAnchor);
                break;
            case MoveOperation::Right:
                m_cursor.movePosition(QTextCursor::MoveOperation::NextCharacter, QTextCursor::KeepAnchor);
                break;
            case MoveOperation::Start:
                m_cursor.movePosition(QTextCursor::MoveOperation::Start, QTextCursor::KeepAnchor);
                break;
            case MoveOperation::End:
                m_cursor.movePosition(QTextCursor::MoveOperation::End, QTextCursor::KeepAnchor);
                break;
            case MoveOperation::StartOfLine:
                m_cursor.movePosition(QTextCursor::MoveOperation::StartOfLine, QTextCursor::KeepAnchor);
                break;
            case MoveOperation::EndOfLine:
                m_cursor.movePosition(QTextCursor::MoveOperation::EndOfLine, QTextCursor::KeepAnchor);
                break;
            case MoveOperation::StartOfWord:
                m_cursor.movePosition(QTextCursor::MoveOperation::StartOfWord, QTextCursor::KeepAnchor);
                break;
            case MoveOperation::EndOfWord:
                m_cursor.movePosition(QTextCursor::MoveOperation::EndOfWord, QTextCursor::KeepAnchor);
                break;
            case MoveOperation::StartOfParagraph:
                m_cursor.movePosition(QTextCursor::MoveOperation::StartOfBlock, QTextCursor::KeepAnchor);
                break;
            case MoveOperation::EndOfParagraph:
                m_cursor.movePosition(QTextCursor::MoveOperation::EndOfBlock, QTextCursor::KeepAnchor);
                break;
            case MoveOperation::StartOfNextLine:
                m_cursor.movePosition(QTextCursor::MoveOperation::StartOfLine, QTextCursor::KeepAnchor);
                m_cursor.movePosition(QTextCursor::MoveOperation::Down, QTextCursor::KeepAnchor);
                break;
            case MoveOperation::StartOfPreviousLine:
                m_cursor.movePosition(QTextCursor::MoveOperation::StartOfLine, QTextCursor::KeepAnchor);
                m_cursor.movePosition(QTextCursor::MoveOperation::Up, QTextCursor::KeepAnchor);
                break;
            case MoveOperation::StartOfNextWord:
                m_cursor.movePosition(QTextCursor::MoveOperation::NextWord, QTextCursor::KeepAnchor);
                break;
            case MoveOperation::StartOfPreviousWord:
                m_cursor.movePosition(QTextCursor::MoveOperation::PreviousWord, QTextCursor::KeepAnchor);
                break;
            case MoveOperation::StartOfNextParagraph:
                m_cursor.movePosition(QTextCursor::MoveOperation::NextBlock, QTextCursor::KeepAnchor);
                break;
            case MoveOperation::StartOfPreviousParagraph:
                m_cursor.movePosition(QTextCursor::MoveOperation::PreviousBlock, QTextCursor::KeepAnchor);
                break;
            default:
                break;
        }
    }

    void TextCursor::selectParagraph() noexcept
    {
        int const pos = m_cursor.position();
        auto const posBlock = m_cursor.document()->findBlock(pos);
        int const anchor = m_cursor.anchor();
        auto const anchorBlock = m_cursor.document()->findBlock(anchor);
        if (pos <= anchor) {
            m_cursor.setPosition(anchorBlock.position() + anchorBlock.length() - 1); // Don't select "\n"
            m_cursor.setPosition(posBlock.position(), QTextCursor::MoveMode::KeepAnchor);
        }
        else {
            m_cursor.setPosition(anchorBlock.position());
            m_cursor.setPosition(posBlock.position() + posBlock.length() - 1, QTextCursor::MoveMode::KeepAnchor);
        }
    }

    std::pair<int, int> TextCursor::getSelection() const noexcept
    {
        return std::pair<int, int>(m_cursor.selectionStart(), m_cursor.selectionEnd());
    }

    bool TextCursor::hasSelection() const noexcept
    {
        return m_cursor.hasSelection();
    }

    QString TextCursor::selectedText() const noexcept
    {
        return m_cursor.selectedText();
    }

    bool TextCursor::contains(int pos) const noexcept
    {
        return m_cursor.selectionStart() <= pos && m_cursor.selectionEnd() >= pos;
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
        m_doc->undoStack().push(
                new internal::TextRemoveCommand(m_doc, m_cursor.selectionStart(), m_cursor.selectedText()));
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
        m_doc->undoStack().push(
                new internal::TextRemoveCommand(m_doc, m_cursor.selectionStart(), m_cursor.selectedText()));
    }

    void TextCursor::deleteSelected() noexcept
    {
        if (!hasSelection())
            return;
        m_doc->undoStack().push(
                new internal::TextRemoveCommand(m_doc, m_cursor.selectionStart(), m_cursor.selectedText()));
    }

    void TextCursor::breakParagraph() noexcept
    {
        if (hasSelection())
            deleteSelected();
        m_doc->undoStack().push(new internal::BlockInsertCommand(m_doc, position()));
    }

    void TextCursor::breakLine() noexcept
    {
        if (hasSelection())
            deleteSelected();
        m_doc->undoStack().push(new internal::TextInsertCommand(m_doc, m_cursor.position(), "\u2028"));
    }

    void TextCursor::insertText(QString text) noexcept
    {
        if (hasSelection()) {
            m_doc->undoStack().beginMacro(Document::tr("replacing text"));
            auto endMacro = gsl::finally([this] { m_doc->undoStack().endMacro(); });
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

    std::vector<TextFormat::WeakId> TextCursor::selectionParagraphFormats() const noexcept
    {
        if (!hasSelection())
            return {paragraphFormat()};

        std::vector<TextFormat::WeakId> formatList;
        for (auto block = m_doc->m_doc->findBlock(m_cursor.selectionStart());
             block.isValid()
                     && block.position() + block.length() >= m_cursor.selectionStart()
                     && block.position() < m_cursor.selectionEnd();
             block = block.next()) {

            auto id = m_doc->formatManager()->getIdOfBlockFormat(block.blockFormat());
            if (formatList.empty() || formatList.front() != id)
                formatList.push_back(id);
        }
        return formatList;
    }

    void TextCursor::setParagraphFormat(TextFormat::WeakId id) noexcept
    {
        if (hasSelection()) {
            m_doc->undoStack().beginMacro(Document::tr("change paragraph format"));
            auto endMacro = gsl::finally([this] { m_doc->undoStack().endMacro(); });
            for (auto block = m_doc->m_doc->findBlock(m_cursor.selectionStart());
                 block.isValid()
                         && block.position() + block.length() >= m_cursor.selectionStart()
                         && block.position() < m_cursor.selectionEnd();
                 block = block.next()) {
                m_doc->undoStack().push(new internal::ParagraphFormatChangeCommand(m_doc, block.blockNumber(), id));
            }
        }
        else
            m_doc->undoStack().push(new internal::ParagraphFormatChangeCommand(m_doc, m_cursor.blockNumber(), id));
    }

    TextFormat::WeakId TextCursor::characterFormat() const noexcept
    {
        return m_doc->m_formatMgr->getIdOfCharFormat(m_cursor.charFormat());
    }

    std::vector<TextFormat::WeakId> TextCursor::selectionCharacterFormats() const noexcept
    {
        if (!hasSelection())
            return {characterFormat()};

        std::vector<TextFormat::WeakId> formatList;
        for (auto block = m_doc->m_doc->findBlock(m_cursor.selectionStart());
             block.isValid()
                     && block.position() + block.length() >= m_cursor.selectionStart()
                     && block.position() < m_cursor.selectionEnd();
             block = block.next()) {

            bool foundOne = false;
            for (auto iter = block.begin(); iter != block.end(); ++iter) {
                if (iter.fragment().isValid()
                        && iter.fragment().position() + iter.fragment().length() > m_cursor.selectionStart()
                        && iter.fragment().position() < m_cursor.selectionEnd()) {
                    foundOne = true;
                    auto id = m_doc->formatManager()->getIdOfCharFormat(iter.fragment().charFormat());
                    if (formatList.empty() || formatList.front() != id)
                        formatList.push_back(id);
                }
                else if (foundOne)
                    break;
            }
        }
        return formatList;
    }

    void TextCursor::setCharacterFormat(TextFormat::WeakId id) noexcept
    {
        if (hasSelection()) {
            m_doc->undoStack().beginMacro(Document::tr("change paragraph format"));
            auto endMacro = gsl::finally([this] { m_doc->undoStack().endMacro(); });

            for (auto block = m_doc->m_doc->findBlock(m_cursor.selectionStart());
                 block.isValid()
                         && block.position() + block.length() >= m_cursor.selectionStart()
                         && block.position() < m_cursor.selectionEnd();
                 block = block.next()) {

                for (auto iter = block.begin();
                     !iter.atEnd();
                     ++iter) {
                    auto fragment = iter.fragment();
                    if (fragment.isValid()
                            && fragment.position() + fragment.length() > m_cursor.selectionStart()
                            && fragment.position() < m_cursor.selectionEnd()) {
                        int startPos = fragment.position();
                        int length = fragment.length();
                        if (startPos < m_cursor.selectionStart())
                        {
                            startPos = m_cursor.selectionStart();
                            length = fragment.length() - (startPos - fragment.position());
                        }
                        if (startPos + length > m_cursor.selectionEnd())
                            length = m_cursor.selectionEnd() - startPos;
                        m_doc->undoStack().push(new internal::CharacterFormatChangeCommand(m_doc, startPos, length, id));
                    }
                }
            }
        }
        else
            m_doc->undoStack().push(new internal::CharacterFormatChangeCommand(m_doc, m_cursor.position(), 0, id));
    }

    void TextCursor::replaceCharacterFormat(TextFormat::WeakId id, TextFormat::WeakId newId) noexcept
    {
        if (!hasSelection())
            return;

        using Fragment = std::pair<int, int>;
        auto findNextFragment = [doc = m_doc->m_doc.get()](int start, int end) -> Fragment {
            QTextCursor cursor(doc);
            cursor.setPosition(start);
            auto block = cursor.block();
            for (auto iter = block.begin(); iter != block.end(); ++iter) {
                if (iter.fragment().contains(start))
                    return {start, std::min(end, iter.fragment().position() + iter.fragment().length())};
            }
            return {start, start};
        };

        auto replaceFormat = [id, newId, doc = m_doc->m_doc.get(), formatMgr = m_doc->formatManager(), &findNextFragment](
                int startPos, int endPos) {
            for (Fragment frag = findNextFragment(startPos, endPos);
                 frag.first != frag.second;
                 frag = findNextFragment(frag.second, endPos)) {
                QTextCursor cursor(doc);
                cursor.setPosition(frag.second);
                auto charFormatId = formatMgr->getIdOfCharFormat(cursor.charFormat());
                if (charFormatId == id) {
                    cursor.setPosition(frag.first, QTextCursor::MoveMode::KeepAnchor);
                    cursor.setCharFormat(*formatMgr->getTextCharFormat(newId));
                }
            }
        };

        auto const startBlock = m_doc->m_doc->findBlock(m_cursor.selectionStart());
        int const startPos = m_cursor.selectionStart();
        int const endPos = startBlock.contains(m_cursor.selectionEnd()) ? m_cursor.selectionEnd()
                                                                        : startBlock.position() + startBlock.length();
        replaceFormat(startPos, endPos);

        if (!startBlock.contains(m_cursor.selectionEnd())) {
            for (auto block = startBlock.next();
                 block.isValid() && block.position() + block.length() < m_cursor.selectionEnd();
                 block = block.next()) {
                replaceFormat(block.position(), block.position() + block.length());
            }

            auto endBlock = m_doc->m_doc->findBlock(m_cursor.selectionEnd());
            replaceFormat(endBlock.position(), m_cursor.selectionEnd());
        }
    }
}