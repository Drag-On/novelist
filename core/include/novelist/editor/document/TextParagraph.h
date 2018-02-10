/**********************************************************
 * @file   TextParagraph.h
 * @author jan
 * @date   2/10/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TEXTPARAGRAPH_H
#define NOVELIST_TEXTPARAGRAPH_H

#include <iterator>
#include <novelist_core_export.h>
#include <QTextBlock>
#include "TextFormatManager.h"

namespace novelist::editor {
    class Document;
    class ParagraphIterator;

    /**
     * Read-only information about a paragraph
     */
    class NOVELIST_CORE_EXPORT TextParagraph {
    public:
        /**
         * @return Paragraph format id
         */
        TextFormatManager::WeakId formatId() const noexcept;

        /**
         * @return Paragraph number when counting from document beginning
         */
        int number() const noexcept;

        /**
         * @return Line number when counting from document beginning
         */
        int firstLineNo() const noexcept;

        /**
         * @return Amount of lines of this paragraph
         */
        int lineCount() const noexcept;

        /**
         * @return Character count (including control characters)
         */
        int length() const noexcept;

        /**
         * Checks if the given position is located within the paragraph
         * @param pos Position to check
         * @return true if within paragraph, otherwise false
         */
        bool contains(int pos) const noexcept;

        /**
         * @return Position of first character in the paragraph
         */
        int position() const noexcept;

        /**
         * @return Text content of the paragraph
         */
        QString text() const noexcept;

        /**
         * @return Bounding rectangle of this paragraph
         */
        QRectF boundingRect() const noexcept;

    private:
        TextParagraph(Document const* doc, QTextBlock block, int lineNo) noexcept;

        Document const* m_doc = nullptr;
        QTextBlock m_block;
        int m_lineNo = 0;

        friend ParagraphIterator;
    };

    /**
     * Constant forward iterator to traverse all paragraphs of a document
     */
    class NOVELIST_CORE_EXPORT ParagraphIterator : public std::iterator<std::forward_iterator_tag, TextParagraph, ptrdiff_t, TextParagraph const*, TextParagraph const&>
    {
    public:
        ParagraphIterator() noexcept = default;
        ParagraphIterator(ParagraphIterator const& other) noexcept;
        ParagraphIterator(ParagraphIterator&& other) noexcept = default;
        ParagraphIterator& operator=(ParagraphIterator const& other) noexcept;
        ParagraphIterator& operator=(ParagraphIterator&& other) noexcept = default;
        ~ParagraphIterator() = default;

        bool operator==(ParagraphIterator const& other) const noexcept;
        bool operator!=(ParagraphIterator const& other) const noexcept;

        ParagraphIterator& operator++() noexcept;
        ParagraphIterator operator++(int) noexcept;
        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        bool valid() const noexcept;

        friend void swap(ParagraphIterator& iter1, ParagraphIterator& iter2) noexcept;
    private:
        ParagraphIterator(Document const* doc, int blockNo) noexcept;

        Document const* m_doc = nullptr;
        int m_blockNo = -1;
        TextParagraph m_par;

        friend TextParagraph;
        friend Document;
    };
}

#endif //NOVELIST_TEXTPARAGRAPH_H
