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
#include "util/MenuHelper.h"
#include "windows/MainWindow.h"
#include "ui_MainWindow.h"

namespace novelist {
    MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags)
            :QMainWindow(parent, flags),
             m_ui{std::make_unique<Ui::MainWindow>()},
             m_model{nullptr}
    {
        setCorner(Qt::Corner::BottomLeftCorner, Qt::DockWidgetArea::LeftDockWidgetArea);
        setCorner(Qt::Corner::TopLeftCorner, Qt::DockWidgetArea::LeftDockWidgetArea);
        setCorner(Qt::Corner::BottomRightCorner, Qt::DockWidgetArea::RightDockWidgetArea);
        setCorner(Qt::Corner::TopRightCorner, Qt::DockWidgetArea::RightDockWidgetArea);

        m_ui->setupUi(this);
        m_ui->sceneTabWidget->useInsightView(m_ui->insightView);
        m_ui->insightView->useSceneTabWidget(m_ui->sceneTabWidget);

        // Replace some actions with appropriate delegate actions
        m_ui->action_Undo = replaceMenuAction(m_ui->menu_Edit, m_ui->action_Undo, &m_undoAction);
        m_ui->action_Redo = replaceMenuAction(m_ui->menu_Edit, m_ui->action_Redo, &m_redoAction);
        m_ui->action_Bold = replaceMenuAndToolbarAction(m_ui->menu_Format, m_ui->toolBarFormat, m_ui->action_Bold,
                m_ui->sceneTabWidget->boldAction());
        m_ui->action_Italic = replaceMenuAndToolbarAction(m_ui->menu_Format, m_ui->toolBarFormat, m_ui->action_Italic,
                m_ui->sceneTabWidget->italicAction());
        m_ui->action_Underline = replaceMenuAndToolbarAction(m_ui->menu_Format, m_ui->toolBarFormat,
                m_ui->action_Underline, m_ui->sceneTabWidget->underlineAction());
        m_ui->action_Overline = replaceMenuAndToolbarAction(m_ui->menu_Format, m_ui->toolBarFormat,
                m_ui->action_Overline, m_ui->sceneTabWidget->overlineAction());
        m_ui->action_Strikethrough = replaceMenuAndToolbarAction(m_ui->menu_Format, m_ui->toolBarFormat,
                m_ui->action_Strikethrough, m_ui->sceneTabWidget->strikethroughAction());
        m_ui->actionSmall_Caps = replaceMenuAndToolbarAction(m_ui->menu_Format, m_ui->toolBarFormat,
                m_ui->actionSmall_Caps, m_ui->sceneTabWidget->smallCapsAction());
        m_ui->actionAdd_Note = replaceMenuAction(m_ui->menu_Inspection, m_ui->actionAdd_Note,
                m_ui->sceneTabWidget->addNoteAction());

        m_ui->retranslateUi(this);

        connect(m_ui->projectView, &ProjectView::modelChanged, this, &MainWindow::onProjectChanged);
        connect(m_ui->projectView, &ProjectView::openSceneRequested, [&](QModelIndex idx) {
            m_ui->sceneTabWidget->openScene(m_ui->projectView->model(), idx);
        });
        connect(m_ui->projectView, &ProjectView::focusReceived, this, &MainWindow::onProjectViewFocus);
        connect(m_ui->sceneTabWidget, &SceneTabWidget::focusReceived, this, &MainWindow::onSceneTabFocus);
        connect(m_ui->menu_Inspection, &QMenu::aboutToShow, this, &MainWindow::onAboutToShowInspectionMenu);

        connect(m_ui->action_New_Project, &QAction::triggered, this, &MainWindow::onNewProject);
        connect(m_ui->action_Open_Project, &QAction::triggered, this, &MainWindow::onOpenProject);
        connect(m_ui->action_Close_Project, &QAction::triggered, this, &MainWindow::onCloseProject);
        connect(m_ui->action_Save, &QAction::triggered, this, &MainWindow::onSaveProject);
        connect(m_ui->actionAbout_Qt, &QAction::triggered, [&]() { QMessageBox::aboutQt(this); });
        connect(m_ui->actionAbout_Novelist, &QAction::triggered, [&]() {
            QMessageBox::about(this, tr("About Novelist"),
                    tr("Novelist is an integrated writing environment for authors."));
        });
    }

    MainWindow::~MainWindow() = default;

    void MainWindow::retranslateUi()
    {
        m_ui->retranslateUi(this);
    }

    void MainWindow::onNewProject()
    {
        if (continueCheckUnsavedChanges()) {
            m_ui->sceneTabWidget->closeAll();
            m_model = std::make_unique<ProjectModel>();
            m_ui->projectView->setModel(m_model.get());
            m_ui->projectView->showProjectPropertiesDialog();
        }
    }

    void MainWindow::onOpenProject()
    {
        if (continueCheckUnsavedChanges()) {
            QFileDialog dialog(this);
            dialog.setFileMode(QFileDialog::Directory);
            dialog.setAcceptMode(QFileDialog::AcceptOpen);
            if (dialog.exec() == QFileDialog::Accepted) {
                auto* m = new ProjectModel;
                if (m->open(dialog.selectedFiles().front())) {
                    m_ui->sceneTabWidget->closeAll();
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
        if (continueCheckUnsavedChanges()) {
            m_ui->sceneTabWidget->closeAll();
            m_ui->projectView->setModel(nullptr);
            m_model.reset();
        }
    }

    void MainWindow::onSaveProject()
    {
        if (m_ui->projectView->model() == nullptr)
            return;

        if (m_ui->projectView->model()->neverSaved()) {
            QFileDialog dialog(this);
            dialog.setFileMode(QFileDialog::Directory);
            dialog.setAcceptMode(QFileDialog::AcceptSave);
            if (dialog.exec() == QFileDialog::Accepted)
                m_ui->projectView->model()->setSaveDir(dialog.selectedFiles().front());
            else
                return;
        }
        if (!m_ui->projectView->model()->save()) {
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
        if (continueCheckUnsavedChanges())
            event->accept();
        else
            event->ignore();
    }

    bool MainWindow::continueCheckUnsavedChanges() const
    {
        if (m_ui->projectView->model() != nullptr && m_ui->projectView->model()->isModified()) {
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Novelist"));
            msgBox.setText(tr("There are unsaved changes."));
            msgBox.setInformativeText(
                    tr("If you continue, all changes will be lost. Are you sure you want to continue?"));
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

    void MainWindow::onProjectChanged(ProjectModel* m)
    {
        if (m == nullptr) {
            m_ui->action_New_Project->setEnabled(true);
            m_ui->action_Open_Project->setEnabled(true);
            m_ui->action_Close_Project->setEnabled(false);
            m_ui->action_Save->setEnabled(false);
            m_ui->action_Undo->setEnabled(false);
            m_ui->action_Redo->setEnabled(false);
        }
        else {
            m_ui->action_New_Project->setEnabled(true);
            m_ui->action_Open_Project->setEnabled(true);
            m_ui->action_Close_Project->setEnabled(true);
            m_ui->action_Save->setEnabled(true);
            m_ui->action_Undo->setEnabled(false);
            m_ui->action_Redo->setEnabled(false);
            m_ui->retranslateUi(this);

            connect(m, &ProjectModel::beforeItemRemoved, this, &MainWindow::onItemAboutToRemoved);
        }
    }

    void MainWindow::onItemAboutToRemoved(QModelIndex const& idx, ProjectModel::NodeType type)
    {
        using NodeType = ProjectModel::NodeType;
        switch (type) {
            case NodeType::Scene: {
                m_ui->sceneTabWidget->closeScene(m_ui->projectView->model(), idx);
                break;
            }
            case NodeType::Chapter: {
                int count = m_model->rowCount(idx);
                for (int i = 0; i < count; ++i)
                    onItemAboutToRemoved(idx.child(i, 0), m_model->nodeType(idx.child(i, 0)));
                break;
            }
            default:
                break;
        }
    }

    void MainWindow::onProjectViewFocus(bool focus)
    {
        if (m_model != nullptr) {
            if (focus) {
                m_undoAction.setDelegate(m_ui->projectView->undoAction());
                m_redoAction.setDelegate(m_ui->projectView->redoAction());
            }
        }
    }

    void MainWindow::onSceneTabFocus(bool focus)
    {
        if (m_model != nullptr) {
            if (focus) {
                m_undoAction.setDelegate(m_ui->sceneTabWidget->undoAction());
                m_redoAction.setDelegate(m_ui->sceneTabWidget->redoAction());
            }
        }
    }

    void MainWindow::onAboutToShowInspectionMenu()
    {
        auto* editor = dynamic_cast<TextEditor*>(m_ui->sceneTabWidget->currentWidget());
        if (editor) {
            QModelIndex insightIdx = editor->insights()->find(editor->textCursor().position());
            if (insightIdx.isValid()) {
                m_ui->menuInsight->clear();
                m_ui->menuInsight->setEnabled(true);
                auto* insight = qvariant_cast<IInsight*>(
                        editor->insights()->data(insightIdx, static_cast<int>(InsightModelRoles::DataRole)));
                absorbMenu(m_ui->menuInsight, const_cast<QMenu*>(&insight->menu()));
                return;
            }
        }
        m_ui->menuInsight->setEnabled(false);
    }
}