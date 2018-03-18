/**********************************************************
 * @file   ParagraphFormatChangeCommand.h
 * @author jan
 * @date   3/18/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_PARAGRAPHFORMATCHANGECOMMAND_H
#define NOVELIST_PARAGRAPHFORMATCHANGECOMMAND_H

#include <QtWidgets/QUndoCommand>
#include "editor/document/Document.h"

namespace novelist::editor::internal {
    class ParagraphFormatChangeCommand : public QUndoCommand {
    public:
        ParagraphFormatChangeCommand(Document* doc, int blockNo, TextFormat::WeakId id) noexcept;

        void undo() override;

        void redo() override;

        int undoPosition() const noexcept;

        int redoPosition() const noexcept;

    private:
        Document* m_doc;
        int m_blockNo;
        TextFormat::WeakId m_formatId;
    };
}

#endif //NOVELIST_PARAGRAPHFORMATCHANGECOMMAND_H
