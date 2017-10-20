/**********************************************************
 * @file   TextMarker.cpp
 * @author jan
 * @date   10/12/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include "document/TextMarker.h"

namespace novelist {
    TextMarker::TextMarker(gsl::not_null<QTextDocument*> doc, int left, int right, QTextCharFormat format)
            :m_doc(doc),
             m_cursor(doc),
             m_format(std::move(format)),
             m_contentsChangeConnection([this] {
                 return connect(m_doc, &QTextDocument::contentsChange, this, &TextMarker::onContentsChange);
             })
    {
        int const textLength = m_doc->toRawText().length();
        if (left < 0 || right < 0 || left > textLength || right > textLength)
            throw std::out_of_range("The range [" + std::to_string(left) + ", " + std::to_string(right) +
                    "] is not in the allowed range of the document [0, " + std::to_string(textLength) + "].");

        m_cursor.setPosition(left);
        m_cursor.setPosition(right, QTextCursor::MoveMode::KeepAnchor);
        doFormat();
    }

    std::pair<int, int> TextMarker::parRange() const noexcept
    {
        return std::make_pair(m_doc->findBlock(m_cursor.selectionStart()).blockNumber(),
                m_doc->findBlock(m_cursor.selectionEnd()).blockNumber());
    }

    std::pair<int, int> TextMarker::range() const noexcept
    {
        return std::make_pair(m_cursor.selectionStart(), m_cursor.selectionEnd());
    }

    void TextMarker::setRange(int left, int right)
    {
        int const textLength = m_doc->toRawText().length();
        if (left < 0 || right < 0 || left > textLength || right > textLength)
            throw std::out_of_range("The range [" + std::to_string(left) + ", " + std::to_string(right) +
                    "] is not in the allowed range of the document [0, " + std::to_string(textLength) + "].");

        m_cursor.setPosition(left);
        m_cursor.setPosition(right, QTextCursor::MoveMode::KeepAnchor);
        doFormat();
    }

    QTextCharFormat const& TextMarker::format() const noexcept
    {
        return m_format;
    }

    void TextMarker::setFormat(QTextCharFormat format) noexcept
    {
        m_format = std::move(format);
        doFormat();
    }

    int TextMarker::length() const noexcept
    {
        return m_cursor.selectionEnd() - m_cursor.selectionStart();
    }

    QTextCursor TextMarker::toCursor() const noexcept
    {
        return m_cursor;
    }

    void TextMarker::doFormat()
    {
        int firstPos = m_cursor.selectionStart();
        int secondPos = m_cursor.selectionEnd();

        QTextBlock leftBlock = m_doc->findBlock(firstPos);
        QTextBlock rightBlock = m_doc->findBlock(secondPos);
        int leftBlockNo = leftBlock.blockNumber();
        int leftBlockPos = firstPos - leftBlock.position();
        int rightBlockNo = rightBlock.blockNumber();
        int rightBlockPos = secondPos - rightBlock.position();

        doFormat(leftBlockNo, leftBlockPos, rightBlockNo, rightBlockPos);
    }

    void TextMarker::doFormat(int leftBlockNo, int leftBlockPos, int rightBlockNo, int rightBlockPos)
    {
        if (!m_cursor.hasSelection())
                emit collapsed(*this);

        removeOldFormats();

        for (int blockNo = leftBlockNo; blockNo <= rightBlockNo; ++blockNo) {
            auto block = m_doc->findBlockByNumber(blockNo);
            QTextLayout* layout = block.layout();
            if (layout == nullptr)
                continue;
            auto ranges = layout->formats();

            int rightPos = rightBlockPos;
            if (blockNo != rightBlockNo)
                rightPos = block.length();
            int leftPos = leftBlockPos;
            if (blockNo != leftBlockNo)
                leftPos = 0;

            QTextLayout::FormatRange newFormatRange;
            newFormatRange.format = m_format;
            newFormatRange.start = leftPos;
            newFormatRange.length = rightPos - leftPos;

            ranges.push_back(newFormatRange);
            layout->setFormats(ranges);

            m_cachedFormats.emplace_back(internal::CachedFormat{newFormatRange, block});

            m_doc->markContentsDirty(block.position(), block.length());
        }
    }

    void TextMarker::removeOldFormats()
    {
        for (auto& c : m_cachedFormats) {
            if (!c.m_block.isValid())
                continue;

            auto formats = c.m_block.layout()->formats();

            formats.erase(std::remove_if(formats.begin(), formats.end(), [&c](QTextLayout::FormatRange const& r) {
                return r == c.m_range;
            }), formats.end());

            c.m_block.layout()->setFormats(formats);
        }
        m_cachedFormats.clear();
    }

    void TextMarker::onContentsChange(int pos, int erased, int added)
    {
        int firstPos = m_cursor.selectionStart();
        int secondPos = m_cursor.selectionEnd();

        QTextBlock leftBlock = m_doc->findBlock(firstPos);
        QTextBlock rightBlock = m_doc->findBlock(secondPos);
        int leftBlockNo = leftBlock.blockNumber();
        int leftBlockPos = firstPos - leftBlock.position();
        int rightBlockNo = rightBlock.blockNumber();
        int rightBlockPos = secondPos - rightBlock.position();

        QTextBlock firstAffectedBlock = m_doc->findBlock(pos);
        QTextBlock lastAffectedBlock = m_doc->findBlock(pos + std::max(erased, added));

        if ((firstAffectedBlock.blockNumber() >= leftBlockNo && firstAffectedBlock.blockNumber() <= rightBlockNo) ||
                (lastAffectedBlock.blockNumber() >= leftBlockNo && lastAffectedBlock.blockNumber() <= rightBlockNo) ||
                (firstAffectedBlock.blockNumber() < leftBlockNo && lastAffectedBlock.blockNumber() > rightBlockNo))
            doFormat(leftBlockNo, leftBlockPos, rightBlockNo, rightBlockPos);
    }

    std::ostream& operator<<(std::ostream& s, TextMarker const& m) noexcept
    {
        s << "[" << m.m_cursor.selectionStart() << ", " << m.m_cursor.selectionEnd() << "]";
        return s;
    }
}