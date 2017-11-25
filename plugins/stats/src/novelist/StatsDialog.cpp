/**********************************************************
 * @file   StatsDialog.cpp
 * @author jan
 * @date   11/25/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "StatsDialog.h"
#include <QtCore/QEvent>
#include "ui_StatsDialog.h"

namespace novelist {
    StatsDialog::StatsDialog(gsl::not_null<ProjectModel*> project, QWidget* parent, Qt::WindowFlags f)
            :QDialog(parent, f),
             m_ui{std::make_unique<Ui::StatsDialog>()}
    {
        m_ui->setupUi(this);
    }

    StatsDialog::~StatsDialog() = default;

    void StatsDialog::retranslateUi()
    {
        m_ui->retranslateUi(this);
    }

    void StatsDialog::changeEvent(QEvent* event)
    {
        QWidget::changeEvent(event);
        switch (event->type()) {
            case QEvent::LanguageChange:
                retranslateUi();
                break;
            default:
                break;
        }
    }
}