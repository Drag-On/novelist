/**********************************************************
 * @file   TextInsertCommand.h
 * @author jan
 * @date   3/18/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TEXTINSERTCOMMAND_H
#define NOVELIST_TEXTINSERTCOMMAND_H

#include <QtWidgets/QUndoCommand>
#include "editor/document/Document.h"

namespace novelist::editor::internal {
    class TextInsertCommand : public QUndoCommand {
    public:
        TextInsertCommand(Document* doc, int pos, QString added) noexcept;

        void undo() override;

        void redo() override;

        int id() const override;

        bool mergeWith(QUndoCommand const* other) override;

        int undoPosition() const noexcept;

        int redoPosition() const noexcept;

    private:
        Document* m_doc;
        int m_pos;
        QString m_added;
    };
}

#endif //NOVELIST_TEXTINSERTCOMMAND_H
