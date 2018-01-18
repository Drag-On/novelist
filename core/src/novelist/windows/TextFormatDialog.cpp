/**********************************************************
 * @file   TextFormatDialog.cpp
 * @author jan
 * @date   1/18/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtCore/QEvent>
#include "windows/TextFormatDialog.h"
#include "ui_TextFormatDialog.h"

namespace novelist {
    TextFormatDialog::TextFormatDialog(QWidget* parent, Qt::WindowFlags f)
            :QDialog(parent, f),
             m_ui{std::make_unique<Ui::TextFormatDialog>()}
    {
        m_ui->setupUi(this);
    }

    TextFormatDialog::~TextFormatDialog() noexcept = default;

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

    }

    void TextFormatDialog::restoreDefaults()
    {

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
}