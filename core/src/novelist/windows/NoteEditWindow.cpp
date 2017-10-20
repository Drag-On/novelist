/**********************************************************
 * @file   NoteEditWindow.cpp
 * @author jan
 * @date   10/20/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtCore/QEvent>
#include <QPushButton>
#include "windows/NoteEditWindow.h"
#include "ui_NoteEditWindow.h"

namespace novelist {
    NoteEditWindow::NoteEditWindow(bool allowRemove, QWidget* parent, Qt::WindowFlags f)
            :QDialog(parent, f),
             m_ui{std::make_unique<Ui::NoteEditDialog>()}
    {
        m_ui->setupUi(this);
        if (allowRemove) {
            QPushButton* b = m_ui->buttonBox->addButton(tr("Remove"), QDialogButtonBox::ButtonRole::DestructiveRole);
            connect(b, &QPushButton::toggled, [this]{ this->setResult(DialogCode::Removed); this->close(); });
        }
    }

    NoteEditWindow::~NoteEditWindow() = default;

    void NoteEditWindow::retranslateUi()
    {
        m_ui->retranslateUi(this);
    }

    void NoteEditWindow::setText(QString text) noexcept
    {
        m_ui->plainTextEdit->document()->setPlainText(text);
    }

    QString NoteEditWindow::text() const noexcept
    {
        return m_ui->plainTextEdit->toPlainText();
    }

    void NoteEditWindow::changeEvent(QEvent* event)
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