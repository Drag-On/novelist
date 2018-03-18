/**********************************************************
 * @file   BlockInsertCommand.h
 * @author jan
 * @date   3/18/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_BLOCKINSERTCOMMAND_H
#define NOVELIST_BLOCKINSERTCOMMAND_H

#include <QtWidgets/QUndoCommand>
#include "editor/document/Document.h"

namespace novelist::editor::internal {
    class BlockInsertCommand : public QUndoCommand {
    public:
        BlockInsertCommand(Document* doc, int pos) noexcept;

        void undo() override;

        void redo() override;

        int undoPosition() const noexcept;

        int redoPosition() const noexcept;

    private:
        Document* m_doc;
        int m_pos;
    };
}

#endif //NOVELIST_BLOCKINSERTCOMMAND_H
