/**********************************************************
 * @file   TextCursorBase.cpp
 * @author jan
 * @date   3/21/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "editor/document/TextCursorBase.h"

namespace novelist::editor {
    TextCursorBase::TextCursorBase(gsl::not_null<Document*> doc) noexcept
            :TextCursorBase(doc, 0)
    {
    }

    TextCursorBase::TextCursorBase(gsl::not_null<Document*> doc, int pos) noexcept
            :TextCursorBase(doc, pos, pos)
    {
    }

    TextCursorBase::TextCursorBase(gsl::not_null<Document*> doc, int pos, int anchor) noexcept
            :m_doc(doc),
             m_cursor(QTextCursor(doc->m_doc.get()))
    {
        m_cursor.setPosition(anchor);
        m_cursor.setPosition(pos, QTextCursor::MoveMode::KeepAnchor);
    }

    int TextCursorBase::position() const noexcept
    {
        return m_cursor.position();
    }

    int TextCursorBase::anchor() const noexcept
    {
        return m_cursor.anchor();
    }

    void TextCursorBase::setPosition(int pos) noexcept
    {
        if (pos <= 0)
            m_cursor.movePosition(QTextCursor::MoveOperation::Start);
        else if (pos >= m_doc->m_doc->characterCount())
            m_cursor.movePosition(QTextCursor::MoveOperation::End);
        else
            m_cursor.setPosition(pos);
    }

    void TextCursorBase::move(TextCursorBase::MoveOperation op) noexcept
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

    void TextCursorBase::select(int pos, int anchor) noexcept
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

    void TextCursorBase::select(TextCursorBase::MoveOperation op) noexcept
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

    void TextCursorBase::selectParagraph() noexcept
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

    std::pair<int, int> TextCursorBase::selection() const noexcept
    {
        return std::pair<int, int>(m_cursor.selectionStart(), m_cursor.selectionEnd());
    }

    bool TextCursorBase::hasSelection() const noexcept
    {
        return m_cursor.hasSelection();
    }

    QString TextCursorBase::selectedText() const noexcept
    {
        return m_cursor.selectedText();
    }

    std::vector<int> TextCursorBase::selectedParagraphs(bool complete) const noexcept
    {
        std::vector<int> parIds;
        for (auto block = m_doc->m_doc->findBlock(m_cursor.selectionStart());
             block.isValid()
                     && block.position() + (complete ? 0 : block.length()) >= m_cursor.selectionStart()
                     && block.position() + (complete ? block.length() : 0) < m_cursor.selectionEnd();
             block = block.next()) {

            parIds.push_back(block.blockNumber());
        }
        return parIds;
    }

    std::vector<TextCursorBase::FragmentData> TextCursorBase::selectedFragments(bool complete) const noexcept
    {
        std::vector<TextCursorBase::FragmentData> fragments;
        auto parIds = selectedParagraphs();
        auto select = selection();

        for (int p : parIds) {
            auto block = m_doc->m_doc->findBlockByNumber(p);

            for (auto iter = block.begin();
                 !iter.atEnd();
                 ++iter) {
                auto fragment = iter.fragment();
                if (fragment.isValid()
                        && fragment.position() + (complete ? 0 : fragment.length()) > select.first
                        && fragment.position() + (complete ? fragment.length() : 0) < select.second) {
                    auto formatId = document()->formatManager()->getIdOfCharFormat(fragment.charFormat());
                    if (!fragments.empty() && fragments.back().m_formatId == formatId)
                        fragments.back().m_endPos = fragment.position() + fragment.length();
                    else
                        fragments.push_back(FragmentData{fragment.position(), fragment.position() + fragment.length(),
                                                         formatId});
                }
            }
        }

        return fragments;
    }

    bool TextCursorBase::contains(int pos) const noexcept
    {
        return m_cursor.selectionStart() <= pos && m_cursor.selectionEnd() >= pos;
    }

    bool TextCursorBase::atParagraphStart() const noexcept
    {
        return m_cursor.atBlockStart();
    }

    bool TextCursorBase::atParagraphEnd() const noexcept
    {
        return m_cursor.atBlockEnd();
    }

    bool TextCursorBase::atStart() const noexcept
    {
        return m_cursor.atStart();
    }

    bool TextCursorBase::atEnd() const noexcept
    {
        return m_cursor.atEnd();
    }

    void TextCursorBase::deletePrevious() noexcept
    {
        if (atStart())
            return;
        if (!hasSelection())
            m_cursor.movePosition(QTextCursor::MoveOperation::PreviousCharacter, QTextCursor::KeepAnchor);
        deleteSelected();
    }

    void TextCursorBase::deleteNext() noexcept
    {
        if (atEnd())
            return;
        if (!hasSelection())
            m_cursor.movePosition(QTextCursor::MoveOperation::NextCharacter, QTextCursor::KeepAnchor);
        deleteSelected();
    }

    void TextCursorBase::deleteSelected() noexcept
    {
        if (!hasSelection())
            return;

        auto const charFormat = m_cursor.charFormat();
        m_cursor.removeSelectedText();

        // If this leaves the block empty make sure the block char format doesn't jump to something set previously,
        // but instead retain the last value
        if (m_cursor.atBlockStart() && m_cursor.atBlockEnd()) {
            m_cursor.setBlockCharFormat(charFormat);
            // Workaround. See TextCursor::setCharacterFormat().
            m_cursor.insertText(" ");
            m_cursor.deletePreviousChar();
        }
    }

    void TextCursorBase::breakParagraph() noexcept
    {
        if (hasSelection())
            deleteSelected();

        auto const prevCharFormat = m_cursor.charFormat();
        m_cursor.insertBlock();
        m_cursor.setBlockCharFormat(prevCharFormat);
    }

    void TextCursorBase::breakLine() noexcept
    {
        if (hasSelection())
            deleteSelected();

        insertText("\u2028");
    }

    void TextCursorBase::insertText(QString text) noexcept
    {
        m_cursor.insertText(text);
    }

    TextFormat::WeakId TextCursorBase::paragraphFormat() const noexcept
    {
        return m_doc->m_formatMgr->getIdOfBlockFormat(m_cursor.blockFormat());
    }

    std::vector<TextFormat::WeakId> TextCursorBase::selectionParagraphFormats() const noexcept
    {
        if (!hasSelection())
            return {paragraphFormat()};

        auto parIds = selectedParagraphs();
        std::vector<TextFormat::WeakId> formatList;
        for (int p : parIds) {
            auto block = m_doc->m_doc->findBlockByNumber(p);
            auto id = m_doc->formatManager()->getIdOfBlockFormat(block.blockFormat());
            if (formatList.empty() || formatList.front() != id)
                formatList.push_back(id);
        }
        return formatList;
    }

    void TextCursorBase::setParagraphFormat(TextFormat::WeakId id) noexcept
    {
        m_cursor.setBlockFormat(*m_doc->m_formatMgr->getTextBlockFormat(id));
        m_doc->onParagraphFormatChanged(m_cursor.blockNumber());
    }

    TextFormat::WeakId TextCursorBase::characterFormat() const noexcept
    {
        return m_doc->m_formatMgr->getIdOfCharFormat(m_cursor.charFormat());
    }

    std::vector<TextFormat::WeakId> TextCursorBase::selectionCharacterFormats() const noexcept
    {
        if (!hasSelection())
            return {characterFormat()};

        auto parIds = selectedParagraphs();
        std::vector<TextFormat::WeakId> formatList;
        for (int p : parIds) {
            auto block = m_doc->m_doc->findBlockByNumber(p);
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

    void TextCursorBase::setCharacterFormat(TextFormat::WeakId id) noexcept
    {
        auto format = *m_doc->m_formatMgr->getTextCharFormat(id);
        if (atParagraphStart() && atParagraphEnd()) {
            m_cursor.setBlockCharFormat(format);
            // Note: The following works around a display bug where changing the block char format makes the whole
            //       editor disappear:
            //       https://bugreports.qt.io/browse/QTBUG-40512
            // TODO: Remove this once above ticket has been resolved.
            m_cursor.insertText(" ");
            m_cursor.deletePreviousChar();
        }
        else
            m_cursor.setCharFormat(format);
    }

    void TextCursorBase::replaceCharacterFormat(TextFormat::WeakId id, TextFormat::WeakId newId) noexcept
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

    QTextCursor const& TextCursorBase::internalCursor() const noexcept
    {
        return m_cursor;
    }

    Document* TextCursorBase::document() noexcept
    {
        return m_doc;
    }

    Document const* TextCursorBase::document() const noexcept
    {
        return m_doc;
    }
}