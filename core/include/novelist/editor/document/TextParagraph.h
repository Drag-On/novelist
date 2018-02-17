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
#include <QTextBlock>
#include <QDebug>
#include <novelist_core_export.h>
#include "TextFormatManager.h"

namespace novelist::editor {
    class Document;
    class TextEditor;
    class TextLine;
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
         * @return Constant reference to the paragraph's lines
         */
        std::vector<TextLine> const& lines() const noexcept;

        /**
         * @return Constant reference to the paragraph's fragments
         */
        std::vector<TextFragment> const& fragments() const noexcept;

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
         * @return Bounding rectangle of this paragraph in the document
         */
        QRectF boundingRect() const noexcept;

        TextParagraph(TextParagraph const& other) noexcept;
        TextParagraph& operator=(TextParagraph const& other) noexcept;

    private:
        TextParagraph(Document const* doc, QTextBlock block, int lineNo) noexcept;

        Document const* m_doc = nullptr;
        QTextBlock m_block;
        int m_lineNo = 0;
        std::vector<TextLine> m_lines;
        std::vector<TextFragment> m_fragments;

        friend TextEditor;
        friend TextLine;
        friend ParagraphIterator;
    };

    /**
     * Read-only data about a single line of a paragraph
     */
    class NOVELIST_CORE_EXPORT TextLine {
    public:
        /**
         * @return The line's ascent distance relative to baseline
         */
        qreal ascent() const noexcept;

        /**
         * @return The line's descent distance relative to baseline
         */
        qreal descent() const noexcept;

        /**
         * @return The line's leading distance
         */
        qreal leading() const noexcept;

        /**
         * @return The line's bounding rectangle relative to the document
         */
        QRectF boundingRect() const noexcept;

        /**
         * @return The line's baseline y coordinate within the document
         */
        qreal baseline() const noexcept;

    private:
        explicit TextLine(TextParagraph* par, QTextLine line) noexcept;

        TextParagraph* m_paragraph;
        QTextLine m_line;

        friend TextParagraph;
    };

    /**
     * Read-only access to formatting of a paragraph
     */
    class NOVELIST_CORE_EXPORT TextFragment {
    public:
        /**
         * @return ID of the character format
         */
        TextFormatManager::WeakId characterFormat() const noexcept;

        /**
         * Check if the requested position is within this fragment
         * @param pos Position to check
         * @return true if position is withing fragment, otherwise false
         */
        bool contains(int pos) const noexcept;

        /**
         * @return Number of characters
         */
        int length() const noexcept;

        /**
         * @return Position of first fragment character in document
         */
        int position() const noexcept;

        /**
         * @return Text of the fragment
         */
        QString text() const noexcept;

    private:
        TextFragment(Document const* doc, QTextFragment fragment) noexcept;

        Document const* m_doc = nullptr;
        QTextFragment m_fragment;

        friend TextParagraph;
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

    QDebug operator<<(QDebug debug, TextParagraph const& p) noexcept;
    QDebug operator<<(QDebug debug, TextLine const& l) noexcept;
    QDebug operator<<(QDebug debug, TextFragment const& f) noexcept;
}

#endif //NOVELIST_TEXTPARAGRAPH_H
