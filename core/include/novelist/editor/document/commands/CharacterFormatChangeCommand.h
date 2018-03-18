/**********************************************************
 * @file   CharacterFormatChangeCommand.h
 * @author jan
 * @date   3/18/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_CHARACTERFORMATCHANGECOMMAND_H
#define NOVELIST_CHARACTERFORMATCHANGECOMMAND_H

#include <QtWidgets/QUndoCommand>
#include "editor/document/Document.h"

namespace novelist::editor::internal {
    class CharacterFormatChangeCommand : public QUndoCommand {
    public:
        CharacterFormatChangeCommand(Document* doc, int pos, int length, TextFormat::WeakId id) noexcept;

        void undo() override;

        void redo() override;

        int undoPosition() const noexcept;

        int redoPosition() const noexcept;

    private:
        Document* m_doc;
        int m_pos;
        int m_length;
        TextFormat::WeakId m_formatId;
    };
}

#endif //NOVELIST_CHARACTERFORMATCHANGECOMMAND_H
