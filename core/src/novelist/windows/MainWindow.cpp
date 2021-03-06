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
#include <random>
#include "windows/SettingsWindow.h"
#include "windows/MainWindow.h"
#include "util/MenuHelper.h"
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

        connect(m_ui->projectView, &ProjectView::modelAboutToChange, [this](ProjectModel* m) {emit projectAboutToChange(m);});
        connect(m_ui->projectView, &ProjectView::modelAboutToChange, this, &MainWindow::onProjectAboutToChange);
        connect(m_ui->projectView, &ProjectView::modelChanged, [this](ProjectModel* m) {emit projectChanged(m);});
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
        connect(m_ui->actionSettings, &QAction::triggered, this, &MainWindow::onOpenSettings);
        connect(m_ui->actionAbout_Qt, &QAction::triggered, [&]() { QMessageBox::aboutQt(this); });
        connect(m_ui->actionAbout_Novelist, &QAction::triggered, [&]() {
            QMessageBox::about(this, tr("About Novelist"),
                    tr("Novelist is an integrated writing environment for authors."));
        });

        retranslateUi();
    }

    MainWindow::~MainWindow()
    {
        // Note: Due to Qt's quirky resource allocation model, the SceneTabWidget will be deallocated after the
        //       ProjectModel. It can then happen that the tab widget tries to access the model, which results in a
        //       SegFault. To circumvent this issue, all tabs are closed here before any other destructor is called.
        //       This way, the tab widget will not try to access the model, as there are no open scenes anymore.
        m_ui->sceneTabWidget->closeAll();
    }

    void MainWindow::retranslateUi()
    {
        m_ui->retranslateUi(this);
    }

    ProjectModel* MainWindow::project() noexcept
    {
        return m_model.get();
    }

    ProjectModel const* MainWindow::project() const noexcept
    {
        return m_model.get();
    }

    SceneTabWidget* MainWindow::sceneTabWidget() noexcept
    {
        return m_ui->sceneTabWidget;
    }

    SceneTabWidget const* MainWindow::sceneTabWidget() const noexcept
    {
        return m_ui->sceneTabWidget;
    }

    ProjectView* MainWindow::projectView() noexcept
    {
        return m_ui->projectView;
    }

    ProjectView const* MainWindow::projectView() const noexcept
    {
        return m_ui->projectView;
    }

    void MainWindow::onNewProject()
    {
        if (continueCheckUnsavedChanges()) {
            m_ui->sceneTabWidget->closeAll();
            m_model = std::make_unique<ProjectModel>();
            m_ui->projectView->setModel(m_model.get());
            if (m_ui->projectView->showProjectPropertiesDialog() == QDialog::DialogCode::Rejected) {
                // If the user aborts the dialog, close the new project again
                m_ui->sceneTabWidget->closeAll();
                m_ui->projectView->setModel(nullptr);
                m_model.reset();
            }
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
#ifdef _WIN32
            // Windows doesn't seem to be able to provide a save dialog for directories. Therefore use the qt dialog.
            dialog.setOptions(QFileDialog::DontUseNativeDialog);
#endif
            if (dialog.exec() == QFileDialog::Accepted)
                m_ui->projectView->model()->setSaveDir(dialog.selectedFiles().front());
            else
                return;
        }
        if (m_ui->projectView->model()->save())
            statusBar()->showMessage(tr("Project successfully saved."), 1000);
        else {
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

    void MainWindow::onOpenSettings()
    {
        SettingsWindow wnd;
        wnd.exec();
    }

    void MainWindow::changeEvent(QEvent* event)
    {
        QMainWindow::changeEvent(event);
        switch (event->type()) {
            case QEvent::LanguageChange:
                retranslateUi();
                emit languageChanged();
                break;
            default:
                break;
        }
    }

    void MainWindow::closeEvent(QCloseEvent* event)
    {
        if (continueCheckUnsavedChanges()) {
            m_ui->projectView->setModel(nullptr);
            event->accept();
        }
        else
            event->ignore();
    }

    void MainWindow::showEvent(QShowEvent* event)
    {
        statusBar()->showMessage(generateWelcomeMessage(), 30000);

        QWidget::showEvent(event);
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

    QString MainWindow::generateWelcomeMessage() const
    {
        static std::vector<QString> messages{
                tr("Welcome!"),
                tr("Hello there, sweetheart. Let's write a book together."),
                tr("Aww, you came back for me!"),
                tr("Isn't it just the perfect day to write a novel?"),
                tr("Hey, you! Yeah, you with the pretty face! Wanna write a novel?"),
                tr("Hi!"),
                tr("Have I ever told you how it tickles when you type?"),
                tr("Welcome back! We're such a great team. You're my best friend!"),
        };
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> dis(0, messages.size() - 1);
        return messages[dis(gen)];
    }

    void MainWindow::onProjectAboutToChange(ProjectModel* m)
    {
        if (m == nullptr) {
            if (auto ptr = dynamic_cast<DelegateAction*>(m_ui->action_Undo); ptr != nullptr) ptr->setDelegate(nullptr);
            if (auto ptr = dynamic_cast<DelegateAction*>(m_ui->action_Redo); ptr != nullptr) ptr->setDelegate(nullptr);
            if (auto ptr = dynamic_cast<DelegateAction*>(m_ui->action_Bold); ptr != nullptr) ptr->setDelegate(nullptr);
            if (auto ptr = dynamic_cast<DelegateAction*>(m_ui->action_Italic); ptr != nullptr) ptr->setDelegate(nullptr);
            if (auto ptr = dynamic_cast<DelegateAction*>(m_ui->action_Underline); ptr != nullptr) ptr->setDelegate(nullptr);
            if (auto ptr = dynamic_cast<DelegateAction*>(m_ui->action_Overline); ptr != nullptr) ptr->setDelegate(nullptr);
            if (auto ptr = dynamic_cast<DelegateAction*>(m_ui->action_Strikethrough); ptr != nullptr) ptr->setDelegate(nullptr);
            if (auto ptr = dynamic_cast<DelegateAction*>(m_ui->actionSmall_Caps); ptr != nullptr) ptr->setDelegate(nullptr);
            if (auto ptr = dynamic_cast<DelegateAction*>(m_ui->actionAdd_Note); ptr != nullptr) ptr->setDelegate(nullptr);
        }
    }

    void MainWindow::onProjectChanged(ProjectModel* m)
    {
        if (m == nullptr) {
            m_ui->menuExport->setEnabled(false);
            m_ui->action_New_Project->setEnabled(true);
            m_ui->action_Open_Project->setEnabled(true);
            m_ui->action_Close_Project->setEnabled(false);
            m_ui->action_Save->setEnabled(false);
            m_ui->action_Undo->setEnabled(false);
            m_ui->action_Redo->setEnabled(false);
        }
        else {
            m_ui->menuExport->setEnabled(!m_ui->menuExport->isEmpty());
            m_ui->action_New_Project->setEnabled(true);
            m_ui->action_Open_Project->setEnabled(true);
            m_ui->action_Close_Project->setEnabled(true);
            m_ui->action_Save->setEnabled(true);
            m_ui->action_Undo->setEnabled(false);
            m_ui->action_Redo->setEnabled(false);

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
                auto* insight = qvariant_cast<Insight*>(
                        editor->insights()->data(insightIdx, static_cast<int>(InsightModelRoles::InsightDataRole)));
                absorbMenu(m_ui->menuInsight, const_cast<QMenu*>(&insight->menu()));
                return;
            }
        }
        m_ui->menuInsight->setEnabled(false);
    }
}