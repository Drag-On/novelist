/**********************************************************
 * @file   Document.h
 * @author jan
 * @date   1/25/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_DOCUMENT_H
#define NOVELIST_DOCUMENT_H

#include <QtCore/QObject>
#include <QtGui/QTextDocument>
#include <QtGui/QTextBlock>
#include <QtWidgets/QUndoStack>
#include <gsl/gsl>
#include <novelist_core_export.h>
#include "TextFormatManager.h"
#include "TextParagraph.h"
#include "Properties.h"

namespace novelist::editor {
    class TextCursor;
    class TextEditor;

    namespace internal {
        class TextInsertCommand;
        class TextRemoveCommand;
        class BlockInsertCommand;
    }

    /**
     * Text document which can be modified through TextCursor objects
     */
    class NOVELIST_CORE_EXPORT Document : public QObject {
    Q_OBJECT

    public:
        /**
         * Constructor
         * @param formatMgr Format manager object
         * @param title Document title
         * @param lang Document language
         * @throw std::invalid_argument if the passed format manager is empty
         */
        explicit Document(gsl::not_null<TextFormatManager*> formatMgr, QString title,
                gsl::not_null<ProjectLanguage const*> lang);

        /**
         * @return Undo stack of this document
         */
        QUndoStack& undoStack() noexcept;

        /**
         * @return Document properties
         */
        Properties& properties() noexcept;

        /**
         * @return Document properties
         */
        Properties const& properties() const noexcept;

        /**
         * @return Raw, unformatted text
         */
        QString toRawText() const noexcept;

        /**
         * @return true if document is empty, otherwise false
         */
        bool empty() const noexcept;

        /**
         * @return Constant iterator to the first paragraph
         */
        ParagraphIterator begin() const noexcept;

        /**
         * @return Constant iterator past the last paragraph
         */
        ParagraphIterator end() const noexcept;

    private:
        void onParagraphFormatChanged(int blockIdx) noexcept;

        void onBlockCountChanged(int newBlockCount) noexcept;

        void onFormatModified(TextFormatManager::WeakId id) noexcept;

        void onFormatReplaced(TextFormatManager::WeakId id, TextFormatManager::WeakId replacementId) noexcept;

        void updateParagraphLayout(QTextBlock block) noexcept;

        void updateIndentWidth() noexcept;

        Properties m_properties;
        gsl::not_null<TextFormatManager*> m_formatMgr;
        std::unique_ptr<QTextDocument> m_doc;
        QUndoStack m_undoStack;

        friend TextCursor;
        friend TextEditor;
        friend TextParagraph;
        friend ParagraphIterator;
        friend internal::TextInsertCommand;
        friend internal::TextRemoveCommand;
        friend internal::BlockInsertCommand;
    };

    namespace internal {
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
}

#endif //NOVELIST_DOCUMENT_H
