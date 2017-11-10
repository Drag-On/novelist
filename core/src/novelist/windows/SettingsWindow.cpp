/**********************************************************
 * @file   SettingsWindow.cpp
 * @author jan
 * @date   11/1/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtCore/QEvent>
#include <QtWidgets/QPushButton>
#include <gsl/gsl>
#include "settings/Settings.h"
#include "windows/SettingsWindow.h"
#include "ui_SettingsWindow.h"

namespace novelist {
    SettingsWindow::SettingsWindow(QWidget* parent, Qt::WindowFlags f)
            :QDialog(parent, f),
             m_ui{std::make_unique<Ui::SettingsDialog>()}
    {
        m_ui->setupUi(this);
        setupPages();

        connect(m_ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingsWindow::apply);
        connect(m_ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked, this,
                &SettingsWindow::restoreDefaults);
        connect(m_ui->pagesListWidget, &QListWidget::currentItemChanged, this, &SettingsWindow::onChangePage);
    }

    void SettingsWindow::setupPages() const noexcept
    {
        QSettings settings;
        for (auto& page : Settings::s_pages) {
            auto* listPageItem = new QListWidgetItem(m_ui->pagesListWidget);
            listPageItem->setText(page->name());
            listPageItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

            auto* widget = page->createWidget();
            settings.beginGroup(page->uid());
            page->initialize(widget, settings);
            settings.endGroup();
            m_ui->pagesStackedWidget->addWidget(widget);
        }
    }

    SettingsWindow::~SettingsWindow() noexcept = default;

    void SettingsWindow::accept()
    {
        apply();
        QDialog::accept();
    }

    void SettingsWindow::reject()
    {
        QDialog::reject();
    }

    void SettingsWindow::apply()
    {
        QSettings settings;
        for (auto iter = Settings::s_pages.begin(); iter != Settings::s_pages.end(); ++iter) {
            settings.beginGroup((*iter)->uid());
            auto idx = gsl::narrow_cast<int>(std::distance(Settings::s_pages.begin(), iter));
            (*iter)->apply(m_ui->pagesStackedWidget->widget(idx), settings);
            (*iter)->initiateUpdate(settings);
            settings.endGroup();
        }
    }

    void SettingsWindow::restoreDefaults()
    {
        int curIdx = m_ui->pagesStackedWidget->currentIndex();
        Settings::s_pages[curIdx]->restoreDefaults(m_ui->pagesStackedWidget->currentWidget());
    }

    void SettingsWindow::retranslateUi()
    {
        m_ui->retranslateUi(this);

        for (int i = 0; i < m_ui->pagesListWidget->count(); ++i)
            m_ui->pagesListWidget->item(i)->setText(Settings::s_pages[i]->name());
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