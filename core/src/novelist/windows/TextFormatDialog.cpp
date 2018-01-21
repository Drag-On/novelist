/**********************************************************
 * @file   TextFormatDialog.cpp
 * @author jan
 * @date   1/18/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <memory>
#include <QtCore/QEvent>
#include "windows/TextFormatDialog.h"
#include "ui_TextFormatDialog.h"

namespace novelist {
    TextFormatDialog::TextFormatDialog(gsl::not_null<TextFormatManager*> mgr, QWidget* parent, Qt::WindowFlags f)
            :QDialog(parent, f),
             m_ui{std::make_unique<Ui::TextFormatDialog>()},
             m_mgr{mgr}
    {
        m_ui->setupUi(this);

        connect(m_ui->pushButtonAddFormat, &QPushButton::pressed, this, &TextFormatDialog::onAddTextFormat);
        connect(m_ui->pushButtonRemoveFormat, &QPushButton::pressed, this, &TextFormatDialog::onRemoveTextFormat);
        connect(m_ui->listWidgetFormats, &QListWidget::itemChanged, this, &TextFormatDialog::onItemChanged);
        connect(m_ui->listWidgetFormats, &QListWidget::itemSelectionChanged, this, &TextFormatDialog::onItemSelectionChanged);
        connect(m_ui->radioButtonAlignLeft, &QRadioButton::toggled, this, &TextFormatDialog::onAlignLeft);
        connect(m_ui->radioButtonAlignRight, &QRadioButton::toggled, this, &TextFormatDialog::onAlignRight);
        connect(m_ui->radioButtonAlignCenter, &QRadioButton::toggled, this, &TextFormatDialog::onAlignCenter);
        connect(m_ui->radioButtonAlignFill, &QRadioButton::toggled, this, &TextFormatDialog::onAlignFill);
        connect(m_ui->spinBoxLeftMargin, qOverload<int>(&QSpinBox::valueChanged), this, &TextFormatDialog::onLeftMarginChanged);
        connect(m_ui->spinBoxRightMargin, qOverload<int>(&QSpinBox::valueChanged), this, &TextFormatDialog::onRightMarginChanged);
        connect(m_ui->spinBoxTopMargin, qOverload<int>(&QSpinBox::valueChanged), this, &TextFormatDialog::onTopMarginChanged);
        connect(m_ui->spinBoxBottomMargin, qOverload<int>(&QSpinBox::valueChanged), this, &TextFormatDialog::onBottomMarginChanged);
        connect(m_ui->spinBoxIndent, qOverload<int>(&QSpinBox::valueChanged), this, &TextFormatDialog::onIndentChanged);
        connect(m_ui->spinBoxTextIndent, qOverload<int>(&QSpinBox::valueChanged), this, &TextFormatDialog::onTextIndentChanged);
        connect(m_ui->checkBoxAutoTextIndent, &QCheckBox::toggled, this, &TextFormatDialog::onAutoTextIndentClicked);
        connect(m_ui->checkBoxBold, &QCheckBox::toggled, this, &TextFormatDialog::onBold);
        connect(m_ui->checkBoxItalic, &QCheckBox::toggled, this, &TextFormatDialog::onItalic);
        connect(m_ui->checkBoxUnderline, &QCheckBox::toggled, this, &TextFormatDialog::onUnderline);
        connect(m_ui->checkBoxOverline, &QCheckBox::toggled, this, &TextFormatDialog::onOverline);
        connect(m_ui->checkBoxStrikethrough, &QCheckBox::toggled, this, &TextFormatDialog::onStrikethrough);
        connect(m_ui->checkBoxSmallCaps, &QCheckBox::toggled, this, &TextFormatDialog::onSmallCaps);

        for (size_t i = 0; i < mgr->size(); ++i) {
            auto format = mgr->getTextFormat(i);
            auto item = std::make_unique<QListWidgetItem>(format->m_name);
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            item->setData(TextFormatIdRole, static_cast<uint32_t>(m_mgr->idFromIndex(i)));
            auto formatCpy = std::make_unique<TextFormatManager::TextFormat>();
            *formatCpy = *format;
            item->setData(TextFormatRole, QVariant::fromValue(formatCpy.release()));
            m_ui->listWidgetFormats->addItem(item.release());
        }

        if (m_ui->listWidgetFormats->count() > 0)
            m_ui->listWidgetFormats->setItemSelected(m_ui->listWidgetFormats->item(0), true);
    }

    TextFormatDialog::~TextFormatDialog() noexcept
    {
        // Every item has a raw pointer to its text format. These objects are deleted here.
        for (int i = 0; i < m_ui->listWidgetFormats->count(); ++i) {
            auto item = m_ui->listWidgetFormats->item(i);
            delete qvariant_cast<TextFormatManager::TextFormat*>(item->data(TextFormatRole));
        }
    }

    void TextFormatDialog::retranslateUi()
    {
        m_ui->retranslateUi(this);
    }

    void TextFormatDialog::accept()
    {
        apply();
        QDialog::accept();
    }

    void TextFormatDialog::reject()
    {
        QDialog::reject();
    }

    void TextFormatDialog::apply()
    {
        for (int i = 0; i < m_ui->listWidgetFormats->count(); ++i) {
            auto item = m_ui->listWidgetFormats->item(i);
            auto format = qvariant_cast<TextFormatManager::TextFormat*>(item->data(TextFormatRole));
            auto idVariant = item->data(TextFormatIdRole);
            if (idVariant.isValid()) {
                // Move item to correct position
                auto id = qvariant_cast<uint32_t>(idVariant);
                m_mgr->setTextFormat(TextFormatManager::FormatId(id), *format);
                auto curIdx = m_mgr->indexFromId(TextFormatManager::FormatId(id));
                m_mgr->move(curIdx, i);
            }
            else {
                // Insert new item
                m_mgr->insert(i, *format);
            }
        }
    }

    void TextFormatDialog::changeEvent(QEvent* event)
    {
        QDialog::changeEvent(event);
        switch (event->type()) {
            case QEvent::LanguageChange:
                retranslateUi();
                break;
            default:
                break;
        }
    }

    void TextFormatDialog::onAddTextFormat()
    {
        auto item = new QListWidgetItem(tr("New text format"));
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setData(TextFormatRole, QVariant::fromValue(new TextFormatManager::TextFormat()));
        m_ui->listWidgetFormats->addItem(item);
        m_ui->listWidgetFormats->editItem(item);
    }

    void TextFormatDialog::onRemoveTextFormat()
    {
        if (!m_ui->listWidgetFormats->selectedItems().empty()) {
            // TODO: Verification that the removed format is not in use
            auto item = m_ui->listWidgetFormats->selectedItems().first();
            delete qvariant_cast<TextFormatManager::TextFormat*>(item->data(TextFormatRole));
            m_ui->listWidgetFormats->removeItemWidget(item);
            delete item; // Yup, this is how Qt rolls
        }
    }

    void TextFormatDialog::onItemChanged(QListWidgetItem* item)
    {
        auto const format = qvariant_cast<TextFormatManager::TextFormat*>(item->data(TextFormatRole));
        format->m_name = item->text();
    }

    void TextFormatDialog::onItemSelectionChanged()
    {
        if (!m_ui->listWidgetFormats->selectedItems().empty()) {
            auto const item = m_ui->listWidgetFormats->selectedItems().first();
            auto const format = qvariant_cast<TextFormatManager::TextFormat*>(item->data(TextFormatRole));

            switch (format->m_alignment) {
                case TextFormatManager::Alignment::Left:
                    m_ui->radioButtonAlignLeft->setChecked(true);
                    break;
                case TextFormatManager::Alignment::Right:
                    m_ui->radioButtonAlignRight->setChecked(true);
                    break;
                case TextFormatManager::Alignment::Center:
                    m_ui->radioButtonAlignCenter->setChecked(true);
                    break;
                case TextFormatManager::Alignment::Fill:
                    m_ui->radioButtonAlignFill->setChecked(true);
                    break;
            }
            m_ui->spinBoxLeftMargin->setValue(format->m_margin.m_left);
            m_ui->spinBoxRightMargin->setValue(format->m_margin.m_right);
            m_ui->spinBoxTopMargin->setValue(format->m_margin.m_top);
            m_ui->spinBoxBottomMargin->setValue(format->m_margin.m_bottom);
            m_ui->spinBoxIndent->setValue(format->m_indentation.m_indent);
            m_ui->spinBoxTextIndent->setValue(format->m_indentation.m_textIndent);
            m_ui->checkBoxAutoTextIndent->setChecked(format->m_indentation.m_autoTextIndent);
            m_ui->checkBoxBold->setChecked(format->m_characterFormat.m_bold);
            m_ui->checkBoxItalic->setChecked(format->m_characterFormat.m_italic);
            m_ui->checkBoxUnderline->setChecked(format->m_characterFormat.m_underline);
            m_ui->checkBoxOverline->setChecked(format->m_characterFormat.m_overline);
            m_ui->checkBoxStrikethrough->setChecked(format->m_characterFormat.m_strikethrough);
            m_ui->checkBoxSmallCaps->setChecked(format->m_characterFormat.m_smallCaps);
        }
    }

    void TextFormatDialog::onAlignLeft()
    {
        auto format = getActiveFormat();
        if (format != nullptr)
            format->m_alignment = TextFormatManager::Alignment::Left;
    }

    void TextFormatDialog::onAlignRight()
    {
        auto format = getActiveFormat();
        if (format != nullptr)
            format->m_alignment = TextFormatManager::Alignment::Right;
    }

    void TextFormatDialog::onAlignCenter()
    {
        auto format = getActiveFormat();
        if (format != nullptr)
            format->m_alignment = TextFormatManager::Alignment::Center;
    }

    void TextFormatDialog::onAlignFill()
    {
        auto format = getActiveFormat();
        if (format != nullptr)
            format->m_alignment = TextFormatManager::Alignment::Fill;
    }

    void TextFormatDialog::onLeftMarginChanged(int val)
    {
        auto format = getActiveFormat();
        if (format != nullptr)
            format->m_margin.m_left = static_cast<uint32_t>(val);
    }

    void TextFormatDialog::onRightMarginChanged(int val)
    {
        auto format = getActiveFormat();
        if (format != nullptr)
            format->m_margin.m_right = static_cast<uint32_t>(val);
    }

    void TextFormatDialog::onTopMarginChanged(int val)
    {
        auto format = getActiveFormat();
        if (format != nullptr)
            format->m_margin.m_top = static_cast<uint32_t>(val);
    }

    void TextFormatDialog::onBottomMarginChanged(int val)
    {
        auto format = getActiveFormat();
        if (format != nullptr)
            format->m_margin.m_bottom = static_cast<uint32_t>(val);
    }

    void TextFormatDialog::onIndentChanged(int val)
    {
        auto format = getActiveFormat();
        if (format != nullptr)
            format->m_indentation.m_indent = static_cast<uint32_t>(val);
    }

    void TextFormatDialog::onTextIndentChanged(int val)
    {
        auto format = getActiveFormat();
        if (format != nullptr)
            format->m_indentation.m_textIndent = static_cast<uint32_t>(val);
    }

    void TextFormatDialog::onAutoTextIndentClicked()
    {
        auto format = getActiveFormat();
        if (format != nullptr)
            format->m_indentation.m_autoTextIndent = m_ui->checkBoxAutoTextIndent->isChecked();
    }

    void TextFormatDialog::onBold()
    {
        auto format = getActiveFormat();
        if (format != nullptr)
            format->m_characterFormat.m_bold = m_ui->checkBoxBold->isChecked();
    }

    void TextFormatDialog::onItalic()
    {
        auto format = getActiveFormat();
        if (format != nullptr)
            format->m_characterFormat.m_italic = m_ui->checkBoxItalic->isChecked();
    }

    void TextFormatDialog::onUnderline()
    {
        auto format = getActiveFormat();
        if (format != nullptr)
            format->m_characterFormat.m_underline = m_ui->checkBoxUnderline->isChecked();
    }

    void TextFormatDialog::onOverline()
    {
        auto format = getActiveFormat();
        if (format != nullptr)
            format->m_characterFormat.m_overline = m_ui->checkBoxOverline->isChecked();
    }

    void TextFormatDialog::onStrikethrough()
    {
        auto format = getActiveFormat();
        if (format != nullptr)
            format->m_characterFormat.m_strikethrough = m_ui->checkBoxStrikethrough->isChecked();
    }

    void TextFormatDialog::onSmallCaps()
    {
        auto format = getActiveFormat();
        if (format != nullptr)
            format->m_characterFormat.m_smallCaps = m_ui->checkBoxSmallCaps->isChecked();
    }

    TextFormatManager::TextFormat* TextFormatDialog::getActiveFormat()
    {
        if (!m_ui->listWidgetFormats->selectedItems().empty()) {
            auto item = m_ui->listWidgetFormats->selectedItems().first();
            auto format = qvariant_cast<TextFormatManager::TextFormat*>(item->data(TextFormatRole));
            return format;
        }
        return nullptr;
    }
}