/**********************************************************
 * @file   TextMarker.h
 * @author jan
 * @date   10/11/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TEXTMARKER_H
#define NOVELIST_TEXTMARKER_H

#include <QtCore/QObject>
#include <QtGui/QTextDocument>
#include <gsl/gsl>
#include <util/Connection.h>

namespace novelist {

    /**
     * Attachment policy for text markers
     */
    enum class TextMarkerAttachment {
        AttachLeft,
        AttachRight,
    };

    /**
     * Marks a position in a document. Marker will stay between its neighboring characters when the documents gets
     * modified, i.e. its position may change. If both neighboring characters are removed, then the marker moves either
     * left or right to the last unmodified position, depending on attachment policy.
     */
    class TextMarker : public QObject {
        Q_OBJECT
    public:
        /**
         * Construct marker
         * @param document Underlying document, pointer must stay valid
         * @param position Global position in that document
         * @param attachment Whether to attach to the left or the right character
         */
        explicit TextMarker(gsl::not_null<QTextDocument*> document, int position,
                TextMarkerAttachment attachment = TextMarkerAttachment::AttachLeft);

        /**
         * Copy constructor
         * @param other Marker to copy
         */
        TextMarker(TextMarker const& other) noexcept;

        /**
         * Copy assignment
         * @param other Marker to copy
         * @return Reference to this
         */
        TextMarker& operator=(TextMarker const& other) noexcept;

        TextMarker(TextMarker&& other) noexcept;

        TextMarker& operator=(TextMarker&& other) noexcept;

        ~TextMarker() override = default;

        /**
         * @return Global position of the marker
         */
        int position() const noexcept;

        /**
         * @return Block number and position within that block
         */
        std::pair<int, int> blockNoAndPos() const noexcept;

        /**
         * @return Marker attachment
         */
        TextMarkerAttachment attachment() const noexcept;

        /**
         * @param attachment New marker attachment
         */
        void setAttachment(TextMarkerAttachment attachment) noexcept;

        bool operator==(TextMarker const& rhs) const;

        bool operator!=(TextMarker const& rhs) const;

        bool operator<(TextMarker const& rhs) const;

        bool operator>(TextMarker const& rhs) const;

        bool operator<=(TextMarker const& rhs) const;

        bool operator>=(TextMarker const& rhs) const;

    signals:
        /**
         * Fired when the text left and right of the marker is erased. The marker then attaches to either the left or
         * the right, depending on attachment policy, but this gives you the option to be informed about it
         * @param pos Position of the marker before removal
         */
        void removed(int pos);

    private:
        QTextDocument* m_doc;
        int m_pos;
        TextMarkerAttachment m_attachment;
        Connection m_onContentsChangeConnection;

    private slots:
        void onContentsChange(int position, int charsRemoved, int charsAdded);
    };
}

#endif //NOVELIST_TEXTMARKER_H
