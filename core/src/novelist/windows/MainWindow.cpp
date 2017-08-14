/**********************************************************
 * @file   MainWindow.cpp
 * @author jan
 * @date   8/12/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <QtCore/QEvent>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include "windows/MainWindow.h"
#include "ui_MainWindow.h"

namespace novelist {
    MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags)
            :QMainWindow(parent, flags),
             m_ui{std::make_unique<Ui::MainWindow>()},
             m_model{nullptr}
    {
        m_ui->setupUi(this);

        connect(m_ui->action_New_Project, &QAction::triggered, this, &MainWindow::onNewProject);
        connect(m_ui->action_Open_Project, &QAction::triggered, this, &MainWindow::onOpenProject);
        connect(m_ui->action_Close_Project, &QAction::triggered, this, &MainWindow::onCloseProject);
        connect(m_ui->action_Save, &QAction::triggered, this, &MainWindow::onSaveProject);
        connect(m_ui->actionAbout_Qt, &QAction::triggered, [&]() { QMessageBox::aboutQt(this); });
        connect(m_ui->actionAbout_Novelist, &QAction::triggered, [&]() {
            QMessageBox::about(this, tr("About Novelist"),
                    tr("Novelist is an integrated writing environment for authors."));
        });
        connect(m_ui->projectView, &ProjectView::openSceneRequested, [&](QModelIndex idx) {
            m_ui->sceneTabWidget->openScene(m_ui->projectView->model(), idx);
        });
    }

    MainWindow::~MainWindow() = default;

    void MainWindow::retranslateUi()
    {
        m_ui->retranslateUi(this);
    }

    void MainWindow::onNewProject()
    {
        if(continueCheckUnsavedChanges())
        {
            QFileDialog dialog(this);
            dialog.setFileMode(QFileDialog::Directory);
            dialog.setAcceptMode(QFileDialog::AcceptSave);
            if(dialog.exec() == QFileDialog::Accepted)
            {
                m_model = std::make_unique<ProjectModel>();
                m_model->setSaveDir(dialog.selectedFiles().front());
                m_ui->projectView->setModel(m_model.get());
                m_ui->projectView->showProjectPropertiesDialog();
                onSaveProject();
            }
        }
    }

    void MainWindow::onOpenProject()
    {
        if(continueCheckUnsavedChanges())
        {
            QFileDialog dialog(this);
            dialog.setFileMode(QFileDialog::Directory);
            dialog.setAcceptMode(QFileDialog::AcceptOpen);
            if(dialog.exec() == QFileDialog::Accepted)
            {
                auto* m = new ProjectModel;
                if(m->open(dialog.selectedFiles().front())) {
                    m_model.reset(m);
                    m_ui->projectView->setModel(m_model.get());
                }
                else {
                    QMessageBox msgBox;
                    msgBox.setWindowTitle(tr("Novelist"));
                    msgBox.setText(tr("Open project failed."));
                    msgBox.setInformativeText(tr("The selected directory does not contain a valid project."));
                    msgBox.setStandardButtons(QMessageBox::Ok);
                    msgBox.setDefaultButton(QMessageBox::Ok);
                    msgBox.setIcon(QMessageBox::Warning);
                    msgBox.exec();
                }
            }
        }
    }

    void MainWindow::onCloseProject()
    {
        if(continueCheckUnsavedChanges())
        {
            m_ui->projectView->setModel(nullptr);
            m_model.reset();
        }
    }

    void MainWindow::onSaveProject()
    {
        if(!(m_ui->projectView->model() != nullptr && m_ui->projectView->model()->save())) {
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Novelist"));
            msgBox.setText(tr("Saving the project failed."));
            msgBox.setInformativeText(tr("Check if you have write privileges."));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
        }

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

    void MainWindow::closeEvent(QCloseEvent* event)
    {
        if(continueCheckUnsavedChanges())
            event->accept();
        else
            event->ignore();
    }

    bool MainWindow::continueCheckUnsavedChanges() const
    {
        if(m_ui->projectView->model() != nullptr && m_ui->projectView->model()->isModified()) {
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Novelist"));
            msgBox.setText(tr("There are unsaved changes."));
            msgBox.setInformativeText(tr("If you continue, all changes will be lost. Are you sure you want to continue?"));
            msgBox.setStandardButtons(QMessageBox::Discard | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Cancel);
            msgBox.setIcon(QMessageBox::Question);
            int ret = msgBox.exec();
            switch (ret) {
                case QMessageBox::Discard:
                    return true;
                case QMessageBox::Cancel:
                default:
                    return false;
            }
        }
        return true;
    }
}