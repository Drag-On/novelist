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
        using TextCursorBase::getSelection;
        using TextCursorBase::hasSelection;
        using TextCursorBase::selectedText;
        using TextCursorBase::contains;
        using TextCursorBase::atParagraphStart;
        using TextCursorBase::atParagraphEnd;
        using TextCursorBase::atStart;
        using TextCursorBase::atEnd;
        using TextCursorBase::deletePrevious;
        using TextCursorBase::deleteNext;
        using TextCursorBase::deleteSelected;
        using TextCursorBase::breakParagraph;
        using TextCursorBase::breakLine;
        using TextCursorBase::insertText;
        using TextCursorBase::paragraphFormat;
        using TextCursorBase::selectionParagraphFormats;
        using TextCursorBase::setParagraphFormat;
        using TextCursorBase::characterFormat;
        using TextCursorBase::selectionCharacterFormats;
        using TextCursorBase::setCharacterFormat;
        using TextCursorBase::replaceCharacterFormat;

    private:
        using TextCursorBase::internalCursor;

        friend QTextCursor const& internal::extractInternalCursor(TextCursor const& cursor) noexcept;
    };
}

#endif //NOVELIST_TEXTCURSOR_H
