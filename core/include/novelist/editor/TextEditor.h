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

    protected:
        void keyPressEvent(QKeyEvent* event) override;

        void keyReleaseEvent(QKeyEvent* event) override;

        bool eventFilter(QObject* obj, QEvent* event) override;

    private:
        std::unique_ptr<Document> m_doc;
        QVBoxLayout* m_vBoxLayout;
        QHBoxLayout* m_hBoxLayout;
        QTextEdit* m_textEdit;
        EditorActions m_actions;
    };
}

#endif //NOVELIST_TEXTEDITOR_H
