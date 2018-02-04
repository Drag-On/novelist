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
        // Signify that this widget handles composite keys (like ô composed from ^ and o)
        setAttribute(Qt::WA_InputMethodEnabled, true);
        m_vBoxLayout = new QVBoxLayout();
        m_hBoxLayout = new QHBoxLayout();
        m_vBoxLayout->setContentsMargins(0, 0, 0, 0);
        m_vBoxLayout->setSpacing(0);
        m_hBoxLayout->setContentsMargins(0, 0, 0, 0);
        m_hBoxLayout->setSpacing(0);
        m_textEdit = new QTextEdit(this);
        m_textEdit->setAttribute(Qt::WA_InputMethodEnabled, false); // Text edit should not bypass our own handling
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

    void TextEditor::keyPressEvent(QKeyEvent* event)
    {
    }

    void TextEditor::keyReleaseEvent(QKeyEvent* event)
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
                else if (keyEvent->matches(QKeySequence::StandardKey::Backspace) || keyEvent->key() == Qt::Key_Backspace)
                    getCursor().deletePrevious();
                else if (keyEvent->matches(QKeySequence::StandardKey::Copy)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::Cut)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::Delete) || keyEvent->key() == Qt::Key_Delete)
                    getCursor().deleteNext();
                else if (keyEvent->matches(QKeySequence::StandardKey::DeleteEndOfLine)) {
                    auto cursor = getCursor();
                    cursor.select(TextCursor::MoveOperation::EndOfLine);
                    cursor.deleteSelected();
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::DeleteEndOfWord)) {
                    auto cursor = getCursor();
                    cursor.select(TextCursor::MoveOperation::EndOfWord);
                    cursor.deleteSelected();
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::DeleteStartOfWord)) {
                    auto cursor = getCursor();
                    cursor.select(TextCursor::MoveOperation::StartOfWord);
                    cursor.deleteSelected();
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::DeleteCompleteLine)) {
                    auto cursor = getCursor();
                    cursor.move(TextCursor::MoveOperation::StartOfLine);
                    cursor.select(TextCursor::MoveOperation::EndOfLine);
                    cursor.deleteSelected();
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::InsertLineSeparator)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::InsertParagraphSeparator))
                    getCursor().insertParagraph();
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToEndOfBlock)) {
                    auto cursor = getCursor();
                    cursor.move(TextCursor::MoveOperation::EndOfParagraph);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToEndOfDocument)) {
                    auto cursor = getCursor();
                    cursor.move(TextCursor::MoveOperation::End);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToEndOfLine)) {
                    auto cursor = getCursor();
                    cursor.move(TextCursor::MoveOperation::EndOfLine);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToNextChar)) {
                    auto cursor = getCursor();
                    cursor.move(TextCursor::MoveOperation::Right);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToNextLine)) {
                    auto cursor = getCursor();
                    cursor.move(TextCursor::MoveOperation::Down);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToNextPage)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToNextWord)) {
                    auto cursor = getCursor();
                    cursor.move(TextCursor::MoveOperation::StartOfNextWord);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToPreviousChar)) {
                    auto cursor = getCursor();
                    cursor.move(TextCursor::MoveOperation::Left);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToPreviousLine)) {
                    auto cursor = getCursor();
                    cursor.move(TextCursor::MoveOperation::Up);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToPreviousPage)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToPreviousWord)) {
                    auto cursor = getCursor();
                    cursor.move(TextCursor::MoveOperation::StartOfPreviousWord);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToStartOfBlock)) {
                    auto cursor = getCursor();
                    cursor.move(TextCursor::MoveOperation::StartOfParagraph);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToStartOfDocument)) {
                    auto cursor = getCursor();
                    cursor.move(TextCursor::MoveOperation::Start);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::MoveToStartOfLine)) {
                    auto cursor = getCursor();
                    cursor.move(TextCursor::MoveOperation::StartOfLine);
                    setCursor(cursor);
                }
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
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectEndOfBlock)) {
                    auto cursor = getCursor();
                    cursor.select(TextCursor::MoveOperation::EndOfParagraph);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectEndOfDocument)) {
                    auto cursor = getCursor();
                    cursor.select(TextCursor::MoveOperation::End);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectEndOfLine)) {
                    auto cursor = getCursor();
                    cursor.select(TextCursor::MoveOperation::EndOfLine);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectNextChar)) {
                    auto cursor = getCursor();
                    cursor.select(TextCursor::MoveOperation::Right);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectNextLine)) {
                    auto cursor = getCursor();
                    cursor.select(TextCursor::MoveOperation::Down);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectNextPage)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectNextWord)) {
                    auto cursor = getCursor();
                    cursor.select(TextCursor::MoveOperation::StartOfNextWord);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectPreviousChar)) {
                    auto cursor = getCursor();
                    cursor.select(TextCursor::MoveOperation::Left);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectPreviousLine)) {
                    auto cursor = getCursor();
                    cursor.select(TextCursor::MoveOperation::Up);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectPreviousPage)) { } // TODO
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectPreviousWord)) {
                    auto cursor = getCursor();
                    cursor.select(TextCursor::MoveOperation::StartOfPreviousWord);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectStartOfBlock)) {
                    auto cursor = getCursor();
                    cursor.select(TextCursor::MoveOperation::StartOfParagraph);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectStartOfDocument)) {
                    auto cursor = getCursor();
                    cursor.select(TextCursor::MoveOperation::Start);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::SelectStartOfLine)) {
                    auto cursor = getCursor();
                    cursor.select(TextCursor::MoveOperation::StartOfLine);
                    setCursor(cursor);
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::ZoomIn)) {
                    QFont font = m_doc->m_formatMgr->getFont();
                    if (font.pointSize() < 48) {
                        font.setPointSize(font.pointSize() + 1);
                        m_doc->m_formatMgr->setFont(font);
                    }
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::ZoomOut)) {
                    QFont font = m_doc->m_formatMgr->getFont();
                    if (font.pointSize() > 5) {
                        font.setPointSize(font.pointSize() - 1);
                        m_doc->m_formatMgr->setFont(font);
                    }
                }
                else if (keyEvent->matches(QKeySequence::StandardKey::FullScreen)) { } // TODO
                else if (keyEvent->text().isEmpty())
                    checkDeadKeyInput(static_cast<Qt::Key>(keyEvent->key()));
                else if (!keyEvent->text().isEmpty())
                    getCursor().insertText(composeInputKey(keyEvent->text()));
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

    bool TextEditor::checkDeadKeyInput(Qt::Key key) noexcept
    {
        // This is a pretty dirty workaround to allow input of combining unicode characters like ^. This should be
        // removed once qt properly supports it.
        //    https://bugreports.qt.io/browse/QTBUG-42181
        //    https://bugreports.qt.io/browse/QTBUG-56452
        m_inputModifier = "";
        switch (key) {
            case Qt::Key::Key_Dead_Abovedot:
                m_inputModifier = QString::fromUtf8("\u0307");
                break;
            case Qt::Key::Key_Dead_Abovering:
                m_inputModifier = QString::fromUtf8("\u030A");
                break;
            case Qt::Key::Key_Dead_Acute:
                m_inputModifier = QString::fromUtf8("\u0301");
                break;
            case Qt::Key::Key_Dead_Belowdot:
                m_inputModifier = QString::fromUtf8("\u0323");
                break;
            case Qt::Key::Key_Dead_Breve:
                m_inputModifier = QString::fromUtf8("\u0306");
                break;
            case Qt::Key::Key_Dead_Caron:
                m_inputModifier = QString::fromUtf8("\u030C");
                break;
            case Qt::Key::Key_Dead_Cedilla:
                m_inputModifier = QString::fromUtf8("\u0327");
                break;
            case Qt::Key::Key_Dead_Circumflex:
                m_inputModifier = QString::fromUtf8("\u0302");
                break;
            case Qt::Key::Key_Dead_Diaeresis:
                m_inputModifier = QString::fromUtf8("\u0308");
                break;
            case Qt::Key::Key_Dead_Doubleacute:
                m_inputModifier = QString::fromUtf8("\u030B");
                break;
            case Qt::Key::Key_Dead_Grave:
                m_inputModifier = QString::fromUtf8("\u0300");
                break;
            case Qt::Key::Key_Dead_Hook:
                m_inputModifier = QString::fromUtf8("\u0309");
                break;
            case Qt::Key::Key_Dead_Horn:
                m_inputModifier = QString::fromUtf8("\u031B");
                break;
            case Qt::Key::Key_Dead_Macron:
                m_inputModifier = QString::fromUtf8("\u0304");
                break;
            case Qt::Key::Key_Dead_Ogonek:
                m_inputModifier = QString::fromUtf8("\u0328");
                break;
            case Qt::Key::Key_Dead_Tilde:
                m_inputModifier = QString::fromUtf8("\u0303");
                break;
            case Qt::Key::Key_Dead_Iota:
            case Qt::Key::Key_Dead_Semivoiced_Sound:
            case Qt::Key::Key_Dead_Voiced_Sound:
            default:
                break;
        }
        return !m_inputModifier.isEmpty();
    }

    QString TextEditor::composeInputKey(QString input) noexcept
    {
        input += m_inputModifier;
        m_inputModifier = "";
        return input;
    }

    QVariant TextEditor::inputMethodQuery(Qt::InputMethodQuery query) const
    {
        return m_textEdit->inputMethodQuery(query);
    }

    void TextEditor::inputMethodEvent(QInputMethodEvent* event)
    {
        if (!event->commitString().isEmpty()) {
            QKeyEvent keyEvent(QEvent::KeyPress, 0, Qt::NoModifier, event->commitString());
            keyPressEvent(&keyEvent);
        }
        event->accept();
    }
}