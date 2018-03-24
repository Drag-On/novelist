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

#include <gsl/gsl>
#include "TextCursorBase.h"
#include "TextFormatManager.h"

namespace novelist::editor {
    class Document;
    class TextEditor;

    namespace internal {
        QTextCursor const& extractInternalCursor(TextCursor const& cursor) noexcept;
    }

    /**
     * Allows to modify contents of a document
     */
    class TextCursor : private TextCursorBase {
    public:
        using MoveOperation = TextCursorBase::MoveOperation;

        using TextCursorBase::TextCursorBase;
        using TextCursorBase::position;
        using TextCursorBase::anchor;
        using TextCursorBase::setPosition;
        using TextCursorBase::move;
        using TextCursorBase::select;
        using TextCursorBase::selectParagraph;
        using TextCursorBase::selection;
        using TextCursorBase::hasSelection;
        using TextCursorBase::selectedText;
        using TextCursorBase::selectedParagraphs;
        using TextCursorBase::contains;
        using TextCursorBase::atParagraphStart;
        using TextCursorBase::atParagraphEnd;
        using TextCursorBase::atStart;
        using TextCursorBase::atEnd;
        using TextCursorBase::deletePrevious;
        using TextCursorBase::deleteNext;
        using TextCursorBase::breakLine;
        using TextCursorBase::paragraphFormat;
        using TextCursorBase::selectionParagraphFormats;
        using TextCursorBase::characterFormat;
        using TextCursorBase::selectionCharacterFormats;

        void deleteSelected() noexcept override;

        void breakParagraph() noexcept override;

        void insertText(QString text) noexcept override;

        void setParagraphFormat(TextFormat::WeakId id) noexcept override;

        void setCharacterFormat(TextFormat::WeakId id) noexcept override;

        void replaceCharacterFormat(TextFormat::WeakId id, TextFormat::WeakId newId) noexcept override;

    private:
        using TextCursorBase::internalCursor;

        friend QTextCursor const& internal::extractInternalCursor(TextCursor const& cursor) noexcept;
    };
}

#endif //NOVELIST_TEXTCURSOR_H
