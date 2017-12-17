/**********************************************************
 * @file   FindWidget.cpp
 * @author jan
 * @date   12/17/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtCore/QEvent>
#include "FindWidget.h"
#include "ui_FindWidget.h"

namespace novelist {
    FindWidget::FindWidget(QWidget* parent) noexcept
            :QWidget(parent),
             m_ui(std::make_unique<Ui::FindWidget>())
    {
        m_ui->setupUi(this);
    }

    void FindWidget::changeEvent(QEvent* event)
    {
        QWidget::changeEvent(event);
        switch (event->type()) {
            case QEvent::LanguageChange:
                m_ui->retranslateUi(this);
                break;
            default:
                break;
        }
    }
}