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
#include <QtWidgets/QLabel>

namespace novelist::editor {

    TextEditorToolBar::TextEditorToolBar(gsl::not_null<QMainWindow*> parent) noexcept
            :QToolBar(tr("Formatting Toolbar"), parent)
    {
        m_parFormatComboBox = new QComboBox(this);
        m_charFormatComboBox = new QComboBox(this);

        m_parFormatComboBox->setToolTip(tr("Format of the paragraph the cursor is in."));
        m_charFormatComboBox->setToolTip(tr("Format of the character the cursor is located at."));

        addWidget(m_parFormatComboBox);
        addWidget(m_charFormatComboBox);

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
            m_cursorSelectionChangedConnection = Connection{
                    [this] {
                        return connect(m_editor, &TextEditor::selectionChanged,
                                       this, &TextEditorToolBar::updateWidgets);
                    }
            };
            m_documentChangedConnection = Connection{
                    [this] {
                        return connect(m_editor, &TextEditor::documentChanged,
                                       this, &TextEditorToolBar::onDocumentChanged);
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

        // If the indexed element is the dynamic "mixed" element, disregard it
        if (!m_parFormatComboBox->itemData(index).isValid())
            return;

        m_editor->getCursor().setParagraphFormat(
                qvariant_cast<TextFormatManager::WeakId>(m_parFormatComboBox->itemData(index)));

        // If there was a "mixed" entry before we can remove it now
        if (!m_parFormatComboBox->itemData(0).isValid())
        {
            m_parFormatComboBox->removeItem(0); // "Mixed"
            m_parFormatComboBox->removeItem(0); // Separator
        }
    }

    void TextEditorToolBar::setCharacterFormat(int index)
    {
        qDebug() << "setCharacterFormat(" << index << ")";

        // If the indexed element is the dynamic "mixed" element, disregard it
        if (!m_charFormatComboBox->itemData(index).isValid())
            return;

        m_editor->getCursor().setCharacterFormat(
                qvariant_cast<TextFormatManager::WeakId>(m_charFormatComboBox->itemData(index)));

        // If there was a "mixed" entry before we can remove it now
        if (!m_charFormatComboBox->itemData(0).isValid())
        {
            m_charFormatComboBox->removeItem(0); // "Mixed"
            m_charFormatComboBox->removeItem(0); // Separator
        }
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

        int digits = 1;
        {
            int max = qMax(1ul, m_mgr->size());
            while (max >= 10) {
                max /= 10;
                ++digits;
            }
        }

        // Populate combo boxes
        for (size_t i = 0; i < m_mgr->size(); ++i) {
            auto const prefix = QString("%1: ").arg(i + 1, digits);
            auto const* format = m_mgr->getTextFormat(i);
            m_parFormatComboBox->addItem(format->m_data.m_icon, prefix + format->m_data.m_name, format->m_id);
            m_charFormatComboBox->addItem(format->m_data.m_icon, prefix + format->m_data.m_name, format->m_id);
        }

        // Select current format
        auto const& cursor = m_editor->getCursor();
        auto const parFormats = cursor.selectionParagraphFormats();
        auto const charFormats = cursor.selectionCharacterFormats();

        qDebug() << parFormats << charFormats;

        if (parFormats.size() == 1)
        {
            size_t parIdx = m_mgr->indexFromId(parFormats.front());
            m_parFormatComboBox->setCurrentIndex(parIdx);
        }
        else
        {
            m_parFormatComboBox->insertSeparator(0);
            m_parFormatComboBox->insertItem(0, tr("Mixed"));
            m_parFormatComboBox->setCurrentIndex(0);
        }
        if (charFormats.size() == 1)
        {
            size_t charIdx = m_mgr->indexFromId(charFormats.front());
            m_charFormatComboBox->setCurrentIndex(charIdx);
        }
        else
        {
            m_charFormatComboBox->insertSeparator(0);
            m_charFormatComboBox->insertItem(0, tr("Mixed"));
            m_charFormatComboBox->setCurrentIndex(0);
        }
    }
}