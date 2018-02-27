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
    class TextEditor;

    /**
     * Allows to modify contents of a document
     */
    class TextCursor {
    public:
        /**
         * Predefined move operations
         */
        enum class MoveOperation {
            None = 0,
            Up,
            Down,
            Left,
            Right,
            Start,
            End,
            StartOfLine,
            EndOfLine,
            StartOfWord,
            EndOfWord,
            StartOfParagraph,
            EndOfParagraph,
            StartOfNextLine,
            StartOfPreviousLine,
            StartOfNextWord,
            StartOfPreviousWord,
            StartOfNextParagraph,
            StartOfPreviousParagraph,
        };

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
         * @return Current anchor position
         */
        int anchor() const noexcept;

        /**
         * @param pos New cursor position
         */
        void setPosition(int pos) noexcept;

        /**
         * Move cursor relative from current position
         * @param op Move operation
         */
        void move(MoveOperation op) noexcept;

        /**
         * Select a stretch of text
         * @param pos Position
         * @param anchor Anchor
         */
        void select(int pos, int anchor) noexcept;

        /**
         * Select text relative from current position / selection
         * @param op
         */
        void select(MoveOperation op) noexcept;

        /**
         * Extends the selection to cover the whole paragraph
         *
         * @details If there was no selection, selects the paragraph the cursor is in. If there was a selection, this
         *          selection is extended in such a way that its start is moved to the beginning of the paragraph it is
         *          in, and its end will be moved to the end of the paragraph it is in.
         */
        void selectParagraph() noexcept;

        /**
         * @return Current selection where the first element is left of the second
         */
        std::pair<int, int> getSelection() const noexcept;

        /**
         * @return true if cursor has a selection, otherwise false
         */
        bool hasSelection() const noexcept;

        /**
         * @return The currently selected text
         */
        QString selectedText() const noexcept;

        /**
         * Checks whether the current selection contains a given position
         *
         * @param pos Position to check
         * @return True in case \p pos is contained in the current selection, otherwise false. If there is no selection,
         *         then this returns true if, and only if the cursor position matches \p pos.
         */
        bool contains(int pos) const noexcept;

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
         * Delete selected text
         */
        void deleteSelected() noexcept;

        /**
         * Insert a new paragraph at the current position using the current paragraph & character format
         */
        void breakParagraph() noexcept;

        /**
         * End the line without ending the paragraph
         */
        void breakLine() noexcept;

        /**
         * Insert text at the current position using the current character format
         * @param text Text to insert
         */
        void insertText(QString text) noexcept;

        /**
         * @return Paragraph format of the paragraph the cursor is located in
         */
        TextFormat::WeakId paragraphFormat() const noexcept;

        /**
         * @return A list of all paragraph formats of the current selection in left-to-right order. If selection is
         *         empty, then the list will contain one element that is identical to that returned by
         *         paragraphFormat().
         * @note   This may contain the same format multiple times, if it appears multiple times within the selection.
         */
        std::vector<TextFormat::WeakId> selectionParagraphFormats() const noexcept;

        /**
         * Changes the format of the current paragraph
         * @param id New format
         */
        void setParagraphFormat(TextFormat::WeakId id) noexcept;

        /**
         * @return Character format of the character right before the cursor.
         */
        TextFormat::WeakId characterFormat() const noexcept;

        /**
         * @return A list of all character formats of the current selection in left-to-right order. If selection is
         *         empty, then the list will contain one element that is identical to that returned by
         *         characterFormat().
         * @note   This may contain the same format multiple times, if it appears multiple times within the selection.
         */
        std::vector<TextFormat::WeakId> selectionCharacterFormats() const noexcept;

        /**
         * Changes the current character format. If this cursor has a selection, changes the character format of the
         * text within that selection.
         * @param id New character format
         */
        void setCharacterFormat(TextFormat::WeakId id) noexcept;

        /**
         * Replace a character format in the selection with another format
         * @param id Format to replace
         * @param newId Replacement format
         */
        void replaceCharacterFormat(TextFormat::WeakId id, TextFormat::WeakId newId) noexcept;

    private:
        Document* m_doc;
        QTextCursor m_cursor;

        friend TextEditor;
    };
}

#endif //NOVELIST_TEXTCURSOR_H
