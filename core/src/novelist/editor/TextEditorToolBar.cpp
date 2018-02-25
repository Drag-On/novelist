/**********************************************************
 * @file   TextEditorToolBar.cpp
 * @author jan
 * @date   2/24/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "editor/TextEditorToolBar.h"
#include <QMainWindow>

namespace novelist::editor {
    TextEditorToolBar::TextEditorToolBar(gsl::not_null<QMainWindow*> parent) noexcept
            :QToolBar(tr("Formatting Toolbar"), parent)
    {
        m_parFormatComboBox = new QComboBox(this);
        m_charFormatComboBox = new QComboBox(this);

        addWidget(m_parFormatComboBox);
        addWidget(m_charFormatComboBox);

        m_documentChangedConnection = Connection{
                [this] {
                    return connect(m_editor, &TextEditor::documentChanged,
                                   this, &TextEditorToolBar::onDocumentChanged);
                }
        };
        m_paragraphIndexActivatedConnection = Connection{
                [this] {
                    return connect(m_parFormatComboBox, QOverload<int>::of(&QComboBox::activated),
                                   this, &TextEditorToolBar::setParagraphFormat);
                }
        };
        m_characterIndexActivatedConnection = Connection{
                [this] {
                    return connect(m_charFormatComboBox, QOverload<int>::of(&QComboBox::activated),
                                   this, &TextEditorToolBar::setCharacterFormat);
                }
        };

        enableFormats(false);
    }

    void TextEditorToolBar::setEditor(TextEditor* editor) noexcept
    {
        m_editor = editor;
        onDocumentChanged(m_editor->getDocument());
        if (m_editor != nullptr) {
            m_cursorPositionChangedConnection = Connection{
                    [this] {
                        return connect(m_editor, &TextEditor::cursorPositionChanged,
                                       this, &TextEditorToolBar::updateWidgets);
                    }
            };
        }
        else
            m_cursorPositionChangedConnection.disconnect();
    }

    TextEditor* TextEditorToolBar::editor() const noexcept
    {
        return m_editor;
    }

    void TextEditorToolBar::onDocumentChanged(Document* document)
    {
        bool const enable = document != nullptr;
        if (enable)
            m_mgr = document->formatManager();
        else
            m_mgr = nullptr;
        enableFormats(enable);
    }

    void TextEditorToolBar::setParagraphFormat(int index)
    {
        qDebug() << "setParagraphFormat(" << index << ")";

        Expects(index >= 0 && index < gsl::narrow_cast<int>(m_mgr->size()));

        m_editor->getCursor().setParagraphFormat(
                qvariant_cast<TextFormatManager::WeakId>(m_parFormatComboBox->itemData(index)));
    }

    void TextEditorToolBar::setCharacterFormat(int index)
    {
        qDebug() << "setCharacterFormat(" << index << ")";

        Expects(index >= 0 && index < gsl::narrow_cast<int>(m_mgr->size()));

        m_editor->getCursor().setCharacterFormat(
                qvariant_cast<TextFormatManager::WeakId>(m_charFormatComboBox->itemData(index)));
    }

    void TextEditorToolBar::enableFormats(bool enabled) noexcept
    {
        if (enabled)
            updateWidgets();
        else {
            m_parFormatComboBox->clear();
            m_charFormatComboBox->clear();
        }
        m_parFormatComboBox->setEnabled(enabled);
        m_charFormatComboBox->setEnabled(enabled);
    }

    void TextEditorToolBar::updateWidgets() noexcept
    {
        Expects(m_editor != nullptr);
        Expects(m_mgr != nullptr);

        ConnectionBlocker blockParConnection(m_paragraphIndexActivatedConnection);
        ConnectionBlocker blockCharConnection(m_characterIndexActivatedConnection);

        m_parFormatComboBox->clear();
        m_charFormatComboBox->clear();

        // Populate combo boxes
        for (size_t i = 0; i < m_mgr->size(); ++i) {
            auto const* format = m_mgr->getTextFormat(i);
            m_parFormatComboBox->addItem(format->m_data.m_name, format->m_id);
            m_charFormatComboBox->addItem(format->m_data.m_name, format->m_id);
        }

        // Select current format
        auto const& cursor = m_editor->getCursor();
        size_t parIdx = m_mgr->indexFromId(cursor.paragraphFormat());
        size_t charIdx = m_mgr->indexFromId(cursor.characterFormat());
        m_parFormatComboBox->setCurrentIndex(parIdx);
        m_charFormatComboBox->setCurrentIndex(charIdx);
    }
}