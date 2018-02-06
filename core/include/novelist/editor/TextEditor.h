/**********************************************************
 * @file   TextEditor.h
 * @author jan
 * @date   2/3/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TEXTEDITOR_H__
#define NOVELIST_TEXTEDITOR_H__
// TODO: Change include guards when old text editor is removed

#include <memory>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QTextEdit>
#include "editor/document/TextCursor.h"
#include "editor/document/Document.h"
#include "EditorActions.h"

namespace novelist::editor {
    namespace internal { class TextEdit; }

    class TextEditor : public QWidget {
    Q_OBJECT

    public:
        explicit TextEditor(QWidget* parent = nullptr) noexcept;

        void setDocument(std::unique_ptr<Document> doc) noexcept;

        Document* getDocument() noexcept;

        Document const* getDocument() const noexcept;

        TextCursor getCursor() const;

        void setCursor(TextCursor cursor);

        EditorActions const& editorActions() const noexcept;

        QVariant inputMethodQuery(Qt::InputMethodQuery query) const override;

    protected:
        void keyPressEvent(QKeyEvent* event) override;

        void keyReleaseEvent(QKeyEvent* event) override;

        void inputMethodEvent(QInputMethodEvent* event) override;

        bool eventFilter(QObject* obj, QEvent* event) override;

    private slots:
        void onUndo() noexcept;

        void onRedo() noexcept;

        void onCopy() const noexcept;

        void onCut() noexcept;

        void onPaste() noexcept;

        void onDelete() noexcept;

        void onSelectAll() noexcept;

    private:
        void tryMoveCursorToUndoPos() noexcept;

        void tryMoveCursorToRedoPos() noexcept;

        bool checkDeadKeyInput(Qt::Key key) noexcept;

        QString composeInputKey(QString input) noexcept;

        void updateActionsOnNewDocument() noexcept;

        std::unique_ptr<Document> m_doc;
        QVBoxLayout* m_vBoxLayout;
        QHBoxLayout* m_hBoxLayout;
        internal::TextEdit* m_textEdit;
        EditorActions m_actions;
        QString m_inputModifier;
    };

    namespace internal {
        class TextEdit : public QTextEdit {
        Q_OBJECT

        public:
            using QTextEdit::QTextEdit;
        protected:
            void keyPressEvent(QKeyEvent* e) override
            {
                e->ignore();
            }

            void keyReleaseEvent(QKeyEvent* e) override
            {
                e->ignore();
            }

            bool canInsertFromMimeData(const QMimeData* /*source*/) const override
            {
                // Copy & paste is handled within the wrapping TextEditor class
                return false;
            }

            void insertFromMimeData(const QMimeData* /*source*/) override
            {
                // Copy & paste is handled within the wrapping TextEditor class
            }
        };
    }
}

#endif //NOVELIST_TEXTEDITOR_H
