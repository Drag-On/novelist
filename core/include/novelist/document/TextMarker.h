/**********************************************************
 * @file   TextMarker.h
 * @author jan
 * @date   10/12/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TEXTMARKER_H
#define NOVELIST_TEXTMARKER_H

#include <QObject>
#include <QTextDocument>
#include <QTextLine>
#include <QTextBlock>
#include <gsl/gsl>
#include "util/Connection.h"

namespace novelist {

    namespace internal {
        class CachedFormat;
    }

    /**
     * Marks a stretch of text in a text document, highlighting it with a char format. The underlying document is not
     * modified.
     */
    class TextMarker : public QObject {
    Q_OBJECT

    public:
        /**
         * Construct a marker on a text document
         * @param doc Pointer to the document. Must not be nullptr and is assumed to stay valid throughout the object's
         *            lifetime
         * @param left Left position of the marked range
         * @param right Right position of the marked range
         * @param format Format used to highlight the marker
         * @throw std::out_of_range if the provided range is invalid
         */
        TextMarker(gsl::not_null<QTextDocument*> doc, int left, int right, QTextCharFormat format);

        /**
         * @return Paragraphs spanned by this insight
         */
        std::pair<int, int> parRange() const noexcept;

        /**
         * @return The marked range on the document
         */
        std::pair<int, int> range() const noexcept;

        /**
         * Modify the marked range
         * @param left Left position
         * @param right Right position
         * @throw std::out_of_range if the provided range is invalid
         */
        void setRange(int left, int right);

        /**
         * @return Character format used for highlighting
         */
        QTextCharFormat const& format() const noexcept;

        /**
         * Set a new format for highlighting
         * @param format New character format
         */
        void setFormat(QTextCharFormat format) noexcept;

        /**
         * @return Distance between left and right marker
         */
        int length() const noexcept;

        /**
         * @return A text cursor that spans the marked text
         */
        QTextCursor toCursor() const noexcept;

        /**
         * Formatted output of marker to a stream
         * @param s Stream
         * @param m Marker
         * @return Stream
         */
        friend std::ostream& operator<<(std::ostream& s, TextMarker const& m) noexcept;

    signals:

        /**
         * Fires when the marker collapses to zero length
         * @param marker Marker
         */
        void collapsed(TextMarker& marker);

    private:
        QTextDocument* const m_doc;
        QTextCursor m_cursor;
        QTextCharFormat m_format;
        std::vector<internal::CachedFormat> m_cachedFormats;
        Connection m_contentsChangeConnection;

        void doFormat();

        void doFormat(int leftBlockNo, int leftBlockPos, int rightBlockNo, int rightBlockPos);

        void removeOldFormats();

    private slots:

        void onContentsChange(int pos, int erased, int added);
    };

    namespace internal {
        struct CachedFormat {
            QTextLayout::FormatRange m_range;
            QTextBlock m_block;
        };
    }
}

#endif //NOVELIST_TEXTMARKER_H
