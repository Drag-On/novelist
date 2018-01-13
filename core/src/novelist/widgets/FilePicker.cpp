/**********************************************************
 * @file   FilePicker.cpp
 * @author jan
 * @date   11/4/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtWidgets/QHBoxLayout>
#include "widgets/FilePicker.h"

namespace novelist {
    FilePicker::FilePicker(QWidget* parent, Qt::WindowFlags f) noexcept
            :QWidget(parent, f)
    {
        m_lineEdit = new QLineEdit(this);
        auto const minLineWidth = m_lineEdit->fontMetrics().width("/home/user/Documents/novelist/");
        m_lineEdit->setMinimumWidth(minLineWidth);

        m_button = new QPushButton("...", this);
        auto const buttonWidth = m_button->fontMetrics().width(m_button->text()) + 20;
        m_button->setMinimumWidth(buttonWidth);
        m_button->setMaximumWidth(buttonWidth);
        connect(m_button, &QPushButton::pressed, this, &FilePicker::onFilePick);

        auto layout = new QHBoxLayout(this);
        layout->setMargin(0);
        layout->addWidget(m_lineEdit);
        layout->addWidget(m_button);
        setLayout(layout);
    }

    QString FilePicker::selectedFile() const noexcept
    {
        return m_lineEdit->text();
    }

    void FilePicker::setSelectedFile(QString const& filename) noexcept
    {
        m_lineEdit->setText(filename);
    }

    QFileDialog::FileMode FilePicker::fileMode() const noexcept
    {
        return m_fileMode;
    }

    void FilePicker::setFileMode(QFileDialog::FileMode fileMode) noexcept
    {
        m_fileMode = fileMode;
    }

    QFileDialog::AcceptMode FilePicker::acceptMode() const noexcept
    {
        return m_acceptMode;
    }

    void FilePicker::setAcceptMode(QFileDialog::AcceptMode acceptMode) noexcept
    {
        m_acceptMode = acceptMode;
    }

    QFileDialog::ViewMode FilePicker::viewMode() const noexcept
    {
        return m_viewMode;
    }

    void FilePicker::setViewMode(QFileDialog::ViewMode viewMode) noexcept
    {
        m_viewMode = viewMode;
    }

    void FilePicker::onFilePick()
    {
        QFileDialog dialog(this);
        dialog.setFileMode(m_fileMode);
        dialog.setAcceptMode(m_acceptMode);
        dialog.setViewMode(m_viewMode);
        dialog.setOptions(m_options);
        dialog.selectFile(m_lineEdit->text());
        if (dialog.exec() == QFileDialog::Accepted)
            m_lineEdit->setText(dialog.selectedFiles().first());
    }
}