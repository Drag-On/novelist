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

        QIcon formatLinkIcon;
        formatLinkIcon.addFile(":/icons/chain-linked", QSize(), QIcon::Mode::Normal, QIcon::State::On);
        formatLinkIcon.addFile(":/icons/chain-broken", QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        m_formatsLinkedAction = new QAction(formatLinkIcon, tr("Link paragraph and character format"), this);
        m_formatsLinkedAction->setCheckable(true);

        m_parFormatComboBox->setToolTip(tr("Format of the paragraph the cursor is in."));
        m_charFormatComboBox->setToolTip(tr("Format of the character the cursor is located at."));

        addWidget(m_parFormatComboBox);
        addAction(m_formatsLinkedAction);
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
            if (m_mgr != nullptr) {
                m_formatModifiedConnection = Connection{
                        [this] {
                            return connect(m_mgr, &TextFormatManager::formatModified,
                                           this, &TextEditorToolBar::onFormatModified);
                        }
                };
                m_formatReplacedConnection = Connection{
                        [this] {
                            return connect(m_mgr, &TextFormatManager::formatReplaced,
                                           this, &TextEditorToolBar::onFormatReplaced);
                        }
                };
                m_formatAddedConnection = Connection{
                        [this] {
                            return connect(m_mgr, &TextFormatManager::formatAdded,
                                           this, &TextEditorToolBar::onFormatAdded);
                        }
                };
                m_formatMovedConnection = Connection{
                        [this] {
                            return connect(m_mgr, &TextFormatManager::formatMoved,
                                           this, &TextEditorToolBar::onFormatMoved);
                        }
                };
            }

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

    void TextEditorToolBar::onFormatModified(TextFormatManager::WeakId /*id*/)
    {
        updateWidgets();
    }

    void
    TextEditorToolBar::onFormatReplaced(TextFormatManager::WeakId /*id*/, TextFormatManager::WeakId /*replacement*/)
    {
        updateWidgets();
    }

    void TextEditorToolBar::onFormatAdded(TextFormatManager::WeakId /*id*/)
    {
        updateWidgets();
    }

    void TextEditorToolBar::onFormatMoved(TextFormatManager::WeakId /*id*/, size_t /*srxIdx*/, size_t /*destIdx*/)
    {
        updateWidgets();
    }

    void TextEditorToolBar::setParagraphFormat(int index)
    {
        qDebug() << "setParagraphFormat(" << index << ")";

        // If the indexed element is the dynamic "mixed" element, disregard it
        if (!m_parFormatComboBox->itemData(index).isValid())
            return;

        auto newId = qvariant_cast<TextFormatManager::WeakId>(m_parFormatComboBox->itemData(index));
        auto cursor = m_editor->getCursor();

        // If character format is linked to paragraph format, all character formats that have the old format should be
        // changed to the new format
        if (m_formatsLinkedAction->isChecked()) {
            TextCursor tCursor(m_editor->getDocument());
            tCursor.setPosition(cursor.getSelection().first);
            tCursor.selectParagraph();
            if (tCursor.atParagraphStart() && tCursor.atParagraphEnd())
                tCursor.setCharacterFormat(newId);
            else
            {
                tCursor.replaceCharacterFormat(tCursor.paragraphFormat(), newId);
                while (!tCursor.contains(cursor.getSelection().second)) {
                    tCursor.move(TextCursor::MoveOperation::StartOfNextParagraph);
                    tCursor.selectParagraph();
                    tCursor.replaceCharacterFormat(tCursor.paragraphFormat(), newId);
                }
            }
            updateWidgets();
        }

        cursor.setParagraphFormat(newId);
        updateWidgets();
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
        if (!m_charFormatComboBox->itemData(0).isValid()) {
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

        if (parFormats.size() == 1) {
            size_t parIdx = m_mgr->indexFromId(parFormats.front());
            m_parFormatComboBox->setCurrentIndex(parIdx);
        }
        else {
            m_parFormatComboBox->insertSeparator(0);
            m_parFormatComboBox->insertItem(0, tr("Mixed"));
            m_parFormatComboBox->setCurrentIndex(0);
        }
        if (charFormats.size() == 1) {
            size_t charIdx = m_mgr->indexFromId(charFormats.front());
            m_charFormatComboBox->setCurrentIndex(charIdx);
        }
        else {
            m_charFormatComboBox->insertSeparator(0);
            m_charFormatComboBox->insertItem(0, tr("Mixed"));
            m_charFormatComboBox->setCurrentIndex(0);
        }
    }
}