/**********************************************************
 * @file   TextEditor.cpp
 * @author jan
 * @date   2/3/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "editor/TextEditor.h"
#include <QKeyEvent>

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

    EditorActions const& TextEditor::editorActions() const noexcept
    {
        return m_actions;
    }

    void TextEditor::keyPressEvent(QKeyEvent* event)
    {
        // Note: Here we catch undo/redo signals such that they go through the document's undo-stack rather than
        // to the original QTextEdit, bypassing the wrapper. This way, QTextDocument's inaccessible undo stack and
        // Document's wrapper undo stack should stay in sync.

        if (!m_doc)
            event->ignore();

        if (event->matches(QKeySequence::StandardKey::Undo))
            m_doc->undoStack().undo();
        else if (event->matches(QKeySequence::StandardKey::Redo))
            m_doc->undoStack().redo();
        else
            event->ignore();
    }
}