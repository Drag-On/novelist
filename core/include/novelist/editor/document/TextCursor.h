/**********************************************************
 * @file   TextCursor.h
 * @author jan
 * @date   1/27/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TEXTCURSOR_H
#define NOVELIST_TEXTCURSOR_H

#include <QtCore/QObject>
#include <QtGui/QTextCursor>
#include <gsl/gsl>
#include "TextFormatManager.h"

namespace novelist::editor {
    class Document;

    /**
     * Allows to modify contents of a document
     */
    class TextCursor : public QObject {
    Q_OBJECT

    public:
        /**
         * Constructor
         * @param doc Document to modify
         */
        explicit TextCursor(gsl::not_null<Document*> doc) noexcept;

        /**
         * Constructor
         * @param doc Document to modify
         * @param pos Cursor position in the document
         */
        TextCursor(gsl::not_null<Document*> doc, int pos) noexcept;

        /**
         * Constructor
         * @param doc Document to modify
         * @param pos Cursor position in the document
         * @param anchor Anchor position in the document
         */
        TextCursor(gsl::not_null<Document*> doc, int pos, int anchor) noexcept;

        /**
         * @return Current cursor position
         */
        int position() const noexcept;

        /**
         * @param pos New cursor position
         */
        void setPosition(int pos) noexcept;

        /**
         * Select a stretch of text
         * @param pos Position
         * @param anchor Anchor
         */
        void select(int pos, int anchor) noexcept;

        /**
         * @return Current selection
         */
        std::pair<int, int> getSelection() const noexcept;

        /**
         * @return true if cursor has a selection, otherwise false
         */
        bool hasSelection() const noexcept;

        /**
         * @return true if at paragraph start, otherwise false
         */
        bool atParagraphStart() const noexcept;

        /**
         * @return true if at paragraph end, otherwise false
         */
        bool atParagraphEnd() const noexcept;

        /**
         * @return true if at document start, otherwise false
         */
        bool atStart() const noexcept;

        /**
         * @return true if at document end, otherwise false
         */
        bool atEnd() const noexcept;

        /**
         * Delete previous character
         */
        void deletePrevious() noexcept;

        /**
         * Delete next character
         */
        void deleteNext() noexcept;

        /**
         * Insert a new paragraph at the current position using the current paragraph & character format
         */
        void insertParagraph() noexcept;

        /**
         * Insert text at the current position using the current character format
         * @param text Text to insert
         */
        void insertText(QString text) noexcept;

        /**
         * @return Current paragraph format
         */
        TextFormat::WeakId paragraphFormat() const noexcept;

        /**
         * Changes the format of the current paragraph
         * @param id New format
         */
        void setParagraphFormat(TextFormat::WeakId id) noexcept;

        /**
         * @return Current character format
         */
        TextFormat::WeakId characterFormat() const noexcept;

        /**
         * Changes the current character format. If this cursor has a selection, changes the character format of the
         * text within that selection.
         * @param id New character format
         */
        void setCharacterFormat(TextFormat::WeakId id) noexcept;

    private:
        Document* m_doc;
        QTextCursor m_cursor;
    };
}

#endif //NOVELIST_TEXTCURSOR_H
