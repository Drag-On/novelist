/**********************************************************
 * @file   TextParagraph.cpp
 * @author jan
 * @date   2/10/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "editor/document/TextParagraph.h"
#include "editor/document/Document.h"

namespace novelist::editor {
    TextFormatManager::WeakId TextParagraph::formatId() const noexcept
    {
        return m_doc->m_formatMgr->getIdOfBlockFormat(m_block.blockFormat());
    }

    int TextParagraph::number() const noexcept
    {
        return m_block.blockNumber();
    }

    int TextParagraph::firstLineNo() const noexcept
    {
        return m_lineNo;
    }

    int TextParagraph::lineCount() const noexcept
    {
        return m_block.layout()->lineCount();
    }

    std::vector<TextLine> const& TextParagraph::lines() const noexcept
    {
        return m_lines;
    }

    std::vector<TextFragment> const& TextParagraph::fragments() const noexcept
    {
        return m_fragments;
    }

    int TextParagraph::length() const noexcept
    {
        return m_block.length();
    }

    bool TextParagraph::contains(int pos) const noexcept
    {
        return m_block.contains(pos);
    }

    int TextParagraph::position() const noexcept
    {
        return m_block.position();
    }

    QString TextParagraph::text() const noexcept
    {
        return m_block.text();
    }

    QRectF TextParagraph::boundingRect() const noexcept
    {
        return m_block.layout()->boundingRect();
    }

    TextParagraph::TextParagraph(Document const* doc, QTextBlock block, int lineNo) noexcept
        : m_doc(doc),
          m_block(block),
          m_lineNo(lineNo)
    {
        if (m_block.isValid())
        {
            for (int i = 0; i < m_block.layout()->lineCount(); ++i)
                m_lines.push_back(TextLine(m_block.layout()->lineAt(i)));
            for (auto iter = m_block.begin(); iter != m_block.end(); ++iter)
                m_fragments.push_back(TextFragment(m_doc, iter.fragment()));
        }
    }

    qreal TextLine::ascent() const noexcept
    {
        return m_line.ascent();
    }

    qreal TextLine::descent() const noexcept
    {
        return m_line.descent();
    }

    qreal TextLine::leading() const noexcept
    {
        return m_line.leading();
    }

    QRectF TextLine::boundingRect() const noexcept
    {
        return m_line.rect();
    }

    qreal TextLine::baseline() const noexcept
    {
        return m_line.rect().y() + m_line.ascent();
    }

    TextLine::TextLine(QTextLine line) noexcept
        : m_line(line)
    {
    }

    TextFormatManager::WeakId TextFragment::characterFormat() const noexcept
    {
        return m_doc->m_formatMgr->getIdOfCharFormat(m_fragment.charFormat());
    }

    bool TextFragment::contains(int pos) const noexcept
    {
        return m_fragment.contains(pos);
    }

    int TextFragment::length() const noexcept
    {
        return m_fragment.length();
    }

    int TextFragment::position() const noexcept
    {
        return m_fragment.position();
    }

    QString TextFragment::text() const noexcept
    {
        return m_fragment.text();
    }

    TextFragment::TextFragment(Document const* doc, QTextFragment fragment) noexcept
        : m_doc(doc),
          m_fragment(fragment)
    {
    }

    ParagraphIterator::ParagraphIterator(ParagraphIterator const& other) noexcept
        : m_doc (other.m_doc),
          m_blockNo(other.m_blockNo),
          m_par(other.m_par)
    {
    }

    ParagraphIterator& ParagraphIterator::operator=(ParagraphIterator const& other) noexcept
    {
        m_doc = other.m_doc;
        m_blockNo = other.m_blockNo;
        return *this;
    }

    bool ParagraphIterator::operator==(ParagraphIterator const& other) const noexcept
    {
        return (m_doc == other.m_doc && m_blockNo == other.m_blockNo) || (!valid() && !other.valid());
    }

    bool ParagraphIterator::operator!=(ParagraphIterator const& other) const noexcept
    {
        return !(*this == other);
    }

    ParagraphIterator& ParagraphIterator::operator++() noexcept
    {
        if (valid()) {
            m_par.m_lineNo += m_par.lineCount();
            ++m_blockNo;
            m_par.m_block = m_doc->m_doc->findBlockByNumber(m_blockNo);
        }
        return *this;
    }

    ParagraphIterator ParagraphIterator::operator++(int) noexcept
    {
        auto p = *this;
        if (valid()) {
            m_par.m_lineNo += m_par.lineCount();
            ++m_blockNo;
            m_par.m_block = m_doc->m_doc->findBlockByNumber(m_blockNo);
        }
        return p;
    }

    TextParagraph const& ParagraphIterator::operator*() const noexcept
    {
        return m_par;
    }

    TextParagraph const* ParagraphIterator::operator->() const noexcept
    {
        return &m_par;
    }

    bool ParagraphIterator::valid() const noexcept
    {
        return m_blockNo >= 0 && m_blockNo < m_doc->m_doc->blockCount();
    }

    void swap(ParagraphIterator& iter1, ParagraphIterator& iter2) noexcept
    {
        std::swap(iter1.m_doc, iter2.m_doc);
        std::swap(iter1.m_blockNo, iter2.m_blockNo);
        std::swap(iter1.m_par, iter2.m_par);
    }

    ParagraphIterator::ParagraphIterator(Document const* doc, int blockNo) noexcept
        : m_doc(doc),
          m_blockNo(blockNo),
          m_par(m_doc, m_doc->m_doc->findBlockByNumber(m_blockNo), 0)
    {
    }
}