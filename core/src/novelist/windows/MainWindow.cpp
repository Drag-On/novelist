/**********************************************************
 * @file   MainWindow.cpp
 * @author jan
 * @date   8/12/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <QtCore/QEvent>
#include "windows/MainWindow.h"
#include "ui_MainWindow.h"

namespace novelist
{
    MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags)
            :QMainWindow(parent, flags),
             m_ui{std::make_unique<Ui::MainWindow>()}
    {
        m_ui->setupUi(this);
    }

    MainWindow::~MainWindow() = default;

    void MainWindow::retranslateUi()
    {
        m_ui->retranslateUi(this);
    }

    void MainWindow::changeEvent(QEvent* event)
    {
        QMainWindow::changeEvent(event);
        switch (event->type()) {
            case QEvent::LanguageChange:
                retranslateUi();
                break;
            default:
                break;
        }
    }
}