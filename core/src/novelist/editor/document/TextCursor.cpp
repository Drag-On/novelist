/**********************************************************
 * @file   TextCursor.cpp
 * @author jan
 * @date   1/27/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QTextFragment>
#include "editor/document/TextCursor.h"
#include "editor/document/commands/TextInsertCommand.h"
#include "editor/document/commands/TextRemoveCommand.h"
#include "editor/document/commands/BlockInsertCommand.h"
#include "editor/document/commands/ParagraphFormatChangeCommand.h"
#include "editor/document/commands/CharacterFormatChangeCommand.h"

namespace novelist::editor {

    void TextCursor::deleteSelected() noexcept
    {
        if (!hasSelection())
            return;
        document()->undoStack().push(std::make_unique<internal::TextRemoveCommand>(document(),
                                                                                   selection().first,
                                                                                   selectedText()).release());
    }

    void TextCursor::breakParagraph() noexcept
    {
        if (hasSelection()) {
            document()->undoStack().beginMacro(Document::tr("break paragraph"));
            auto endMacro = gsl::finally([this] { document()->undoStack().endMacro(); });
            deleteSelected();
            document()->undoStack().push(std::make_unique<internal::BlockInsertCommand>(document(),
                                                                                        position()).release());
        }
        else
            document()->undoStack().push(std::make_unique<internal::BlockInsertCommand>(document(),
                                                                                        position()).release());
    }

    void TextCursor::insertText(QString text) noexcept
    {
        if (hasSelection()) {
            document()->undoStack().beginMacro(Document::tr("replacing text"));
            auto endMacro = gsl::finally([this] { document()->undoStack().endMacro(); });
            deleteSelected();
            document()->undoStack().push(std::make_unique<internal::TextInsertCommand>(document(),
                                                                                       selection().first,
                                                                                       std::move(text)).release());
        }
        else
            document()->undoStack().push(std::make_unique<internal::TextInsertCommand>(document(),
                                                                                       position(),
                                                                                       std::move(text)).release());
    }

    void TextCursor::setParagraphFormat(TextFormat::WeakId id) noexcept
    {
        TextCursorBase::setParagraphFormat(id); // TODO:
    }

    void TextCursor::setCharacterFormat(TextFormat::WeakId id) noexcept
    {
        TextCursorBase::setCharacterFormat(id); // TODO:
    }

    void TextCursor::replaceCharacterFormat(TextFormat::WeakId id, TextFormat::WeakId newId) noexcept
    {
        TextCursorBase::replaceCharacterFormat(id, newId); // TODO:
    }
}