/**********************************************************
 * @file   TextEditor.cpp
 * @author jan
 * @date   2/3/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "editor/TextEditor.h"
#include <QDebug>

namespace novelist::editor {
    TextEditor::TextEditor(QWidget* parent) noexcept
            :QWidget(parent)
    {
        m_vBoxLayout = new QVBoxLayout();
        m_hBoxLayout = new QHBoxLayout();
        m_vBoxLayout->setContentsMargins(0, 0, 0, 0);
        m_vBoxLayout->setSpacing(0);
        m_hBoxLayout->setContentsMargins(0, 0, 0, 0);
        m_hBoxLayout->setSpacing(0);
        m_textEdit = new QTextEdit(this);
        m_textEdit->installEventFilter(this);
        m_textEdit->setEnabled(false);
        m_hBoxLayout->addWidget(m_textEdit);
        m_vBoxLayout->addLayout(m_hBoxLayout);
        setLayout(m_vBoxLayout);
    }

    void TextEditor::setDocument(std::unique_ptr<Document> doc) noexcept
    {
        m_doc = std::move(doc);
        if (m_doc) {
            m_textEdit->setDocument(m_doc->m_doc.get());
            m_textEdit->setEnabled(true);
            m_actions.m_undoAction = m_doc->undoStack().createUndoAction(this, tr("Undo"));
            m_actions.m_redoAction = m_doc->undoStack().createRedoAction(this, tr("Redo"));
        }
        else {
            m_textEdit->setDocument(nullptr);
            m_textEdit->setEnabled(false);
        }
    }

    Document* TextEditor::getDocument() noexcept
    {
        return m_doc.get();
    }

    Document const* TextEditor::getDocument() const noexcept
    {
        return m_doc.get();
    }

    TextCursor TextEditor::getCursor() const
    {
        return TextCursor(m_doc.get(), m_textEdit->textCursor().position(), m_textEdit->textCursor().anchor());
    }

    void TextEditor::setCursor(TextCursor cursor)
    {
        m_textEdit->setTextCursor(cursor.m_cursor);
    }

    EditorActions const& TextEditor::editorActions() const noexcept
    {
        return m_actions;
    }

    void TextEditor::keyPressEvent(QKeyEvent* /*event*/)
    {
    }

    void TextEditor::keyReleaseEvent(QKeyEvent* /*event*/)
    {
    }

    bool TextEditor::eventFilter(QObject* obj, QEvent* event)
    {
        if (obj == m_textEdit) {
            // Note: Here we catch key input signals that will modify the underlying document. It is important not to let
            //       the internal QTextEdit handle them, as this would bypass the custom undo-redo framework of Document.
            if (!m_doc)
                return true;

            if (event->type() == QEvent::KeyPress) {
                auto* keyEvent = static_cast<QKeyEvent*>(event);

                if (keyEvent->matches(QKeySequence::StandardKey::Undo)) {
                    if (!m_doc->undoStack().canUndo())
                        return true;
                    m_doc->undoStack().undo();
                    tryMoveCursorToUndoPos();
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::Redo)) {
                    if (!m_doc->undoStack().canRedo())
                        return true;
                    m_doc->undoStack().redo();
                    tryMoveCursorToRedoPos();
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::Backspace) ||
                        keyEvent->key() == Qt::Key_Backspace)
                    getCursor().deletePrevious();
                else if (keyEvent->matches(QKeySequence::StandardKey::Copy)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::Cut)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::Delete) ||
                        keyEvent->key() == Qt::Key_Delete)
                    getCursor().deleteNext();
                else if (keyEvent->matches(QKeySequence::StandardKey::DeleteEndOfLine)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::DeleteEndOfWord)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::DeleteStartOfWord)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::DeleteCompleteLine)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::InsertLineSeparator)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::InsertParagraphSeparator))
                    getCursor().insertParagraph();
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToEndOfBlock)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToEndOfLine)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToNextChar)) {
                    auto cursor = getCursor();
                    cursor.setPosition(cursor.position() + 1);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToNextLine)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToNextPage)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToNextWord)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToPreviousChar)) {
                    auto cursor = getCursor();
                    cursor.setPosition(cursor.position() - 1);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToPreviousLine)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToPreviousPage)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToPreviousWord)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToStartOfBlock)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToStartOfDocument)) {
                    auto cursor = getCursor();
                    cursor.setPosition(0);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToStartOfLine)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::Paste)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::Print)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::Refresh)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectAll)) {
                    auto cursor = getCursor();
                    cursor.select(0, m_doc->m_doc->characterCount());
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::Deselect)) {
                    auto cursor = getCursor();
                    cursor.setPosition(cursor.position());
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectEndOfBlock)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectEndOfDocument)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectEndOfLine)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectNextChar)) {
                    auto cursor = getCursor();
                    cursor.select(cursor.position() + 1, cursor.anchor());
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectNextLine)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectNextPage)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectNextWord)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectPreviousChar)) {
                    auto cursor = getCursor();
                    cursor.select(cursor.position() - 1, cursor.anchor());
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectPreviousLine)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectPreviousPage)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectPreviousWord)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectStartOfBlock)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectStartOfDocument)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectStartOfLine)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::ZoomIn)) {
                    auto font = m_textEdit->font();
                    font.setPointSize(font.pointSize() + 1);
                    m_textEdit->setFont(font);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::ZoomOut)) {
                    auto font = m_textEdit->font();
                    font.setPointSize(font.pointSize() - 1);
                    m_textEdit->setFont(font);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::FullScreen)) { } // TODO
                else if (!keyEvent->text().isEmpty())
                    getCursor().insertText(keyEvent->text());
                return true;
            }
            else if (event->type() == QEvent::KeyRelease) {
                return true;
            }
        }
        return false;
    }

    void TextEditor::tryMoveCursorToUndoPos() noexcept
    {
        int pos = -1;
        auto cmd = m_doc->undoStack().command(m_doc->undoStack().index());
        if (auto ptr = dynamic_cast<internal::TextInsertCommand const*>(cmd); ptr != nullptr)
            pos = ptr->undoPosition();
        else if (auto ptr = dynamic_cast<internal::TextRemoveCommand const*>(cmd); ptr != nullptr)
            pos = ptr->undoPosition();
        else if (auto ptr = dynamic_cast<internal::BlockInsertCommand const*>(cmd); ptr != nullptr)
            pos = ptr->undoPosition();
        if (pos >= 0) {
            auto cursor = getCursor();
            cursor.setPosition(pos);
            setCursor(cursor);
        }
    }

    void TextEditor::tryMoveCursorToRedoPos() noexcept
    {
        int pos = -1;
        auto cmd = m_doc->undoStack().command(m_doc->undoStack().index() - 1);
        if (auto ptr = dynamic_cast<internal::TextInsertCommand const*>(cmd); ptr != nullptr)
            pos = ptr->redoPosition();
        else if (auto ptr = dynamic_cast<internal::TextRemoveCommand const*>(cmd); ptr != nullptr)
            pos = ptr->redoPosition();
        else if (auto ptr = dynamic_cast<internal::BlockInsertCommand const*>(cmd); ptr != nullptr)
            pos = ptr->redoPosition();
        if (pos >= 0) {
            auto cursor = getCursor();
            cursor.setPosition(pos);
            setCursor(cursor);
        }
    }
}