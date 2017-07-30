/**********************************************************
 * @file   ProjectPropertiesWindow.cpp
 * @author jan
 * @date   7/30/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <QtCore/QEvent>
#include "windows/ProjectPropertiesWindow.h"
#include "ui_ProjectPropertiesWindow.h"

namespace novelist {
    ProjectPropertiesWindow::ProjectPropertiesWindow(QWidget* parent, Qt::WindowFlags f)
            :QDialog(parent, f),
             m_ui{std::make_unique<Ui::ProjectPropertiesWindow>()}
    {
        m_ui->setupUi(this);
    }

    ProjectPropertiesWindow::~ProjectPropertiesWindow() = default;

    void ProjectPropertiesWindow::retranslateUi()
    {
        m_ui->retranslateUi(this);
    }

    void ProjectPropertiesWindow::setProperties(ProjectProperties const& properties)
    {
        m_ui->lineEditName->setText(properties.m_name);
        m_ui->lineEditAuthor->setText(properties.m_author);
        m_ui->languagePicker->setCurrentLanguage(properties.m_lang);
    }

    ProjectProperties ProjectPropertiesWindow::properties() const
    {
        ProjectProperties props;
        props.m_name = m_ui->lineEditName->text();
        props.m_author = m_ui->lineEditAuthor->text();
        props.m_lang = m_ui->languagePicker->currentLanguage();
        return props;
    }

    void ProjectPropertiesWindow::changeEvent(QEvent* event)
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