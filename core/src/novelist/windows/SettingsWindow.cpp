/**********************************************************
 * @file   SettingsWindow.cpp
 * @author jan
 * @date   11/1/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtCore/QEvent>
#include <include/gsl/gsl_util>
#include "settings/Settings.h"
#include "windows/SettingsWindow.h"
#include "ui_SettingsWindow.h"

namespace novelist {
    SettingsWindow::SettingsWindow(QWidget* parent, Qt::WindowFlags f)
            :QDialog(parent, f),
             m_ui{std::make_unique<Ui::SettingsDialog>()}
    {
        m_ui->setupUi(this);

        QSettings settings;
        for (auto& page : Settings::s_pages) {
            auto* listPageItem = new QListWidgetItem(m_ui->pagesListWidget);
            listPageItem->setText(page->name());
            listPageItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

            auto* widget = page->createWidget();
            settings.beginGroup(page->uid());
            page->update(widget, settings);
            settings.endGroup();
            m_ui->pagesStackedWidget->addWidget(widget);
        }

        connect(m_ui->pagesListWidget, &QListWidget::currentItemChanged, this, &SettingsWindow::onChangePage);
    }

    SettingsWindow::~SettingsWindow() noexcept = default;

    void SettingsWindow::accept()
    {
        QSettings settings;
        for (auto iter = Settings::s_pages.begin(); iter != Settings::s_pages.end(); ++iter) {
            settings.beginGroup((*iter)->uid());
            auto idx = gsl::narrow_cast<int>(std::distance(Settings::s_pages.begin(), iter));
            (*iter)->apply(m_ui->pagesStackedWidget->widget(idx), settings);
            settings.endGroup();
        }
        QDialog::accept();
    }

    void SettingsWindow::reject()
    {
        QDialog::reject();
    }

    void SettingsWindow::retranslateUi()
    {
        m_ui->retranslateUi(this);
    }

    void SettingsWindow::changeEvent(QEvent* event)
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

    void SettingsWindow::onChangePage(QListWidgetItem* current, QListWidgetItem* /*previous*/)
    {
        m_ui->pagesStackedWidget->setCurrentIndex(m_ui->pagesListWidget->row(current));
    }
}