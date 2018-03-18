/**********************************************************
 * @file   TextRemoveCommand.h
 * @author jan
 * @date   3/18/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TEXTREMOVECOMMAND_H
#define NOVELIST_TEXTREMOVECOMMAND_H

#include <QtWidgets/QUndoCommand>
#include "editor/document/Document.h"

namespace novelist::editor::internal {
    class TextRemoveCommand : public QUndoCommand {
    public:
        TextRemoveCommand(Document* doc, int pos, QString removed) noexcept;

        void undo() override;

        void redo() override;

        int id() const override;

        bool mergeWith(QUndoCommand const* other) override;

        int undoPosition() const noexcept;

        int redoPosition() const noexcept;

    private:
        Document* m_doc;
        int m_pos;
        QString m_removed;
    };
}

#endif //NOVELIST_TEXTREMOVECOMMAND_H
