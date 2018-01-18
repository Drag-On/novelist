/**********************************************************
 * @file   MainWindow.h
 * @author jan
 * @date   8/12/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_MAINWINDOW_H
#define NOVELIST_MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <memory>
#include "util/DelegateAction.h"
#include "util/ConnectionWrapper.h"
#include "model/ProjectModel.h"
#include "widgets/SceneTabWidget.h"
#include "view/ProjectView.h"
#include "novelist_core_export.h"

namespace Ui {
    class MainWindow;
}

namespace novelist {
    class NOVELIST_CORE_EXPORT MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        explicit MainWindow(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

        ~MainWindow() noexcept override;

        /**
         * Translate UI to new language
         */
        void retranslateUi();

        /**
         * @return Pointer to the currently active project. May be nullptr.
         */
        ProjectModel* project() noexcept;

        /**
         * @return Pointer to the currently active project. May be nullptr.
         */
        ProjectModel const* project() const noexcept;

        /**
         * @return Pointer to the scene tab widget
         */
        SceneTabWidget* sceneTabWidget() noexcept;

        /**
         * @return Pointer to the scene tab widget
         */
        SceneTabWidget const* sceneTabWidget() const noexcept;

        /**
         * @return Pointer to the project view
         */
        ProjectView* projectView() noexcept;

        /**
         * @return Pointer to the project view
         */
        ProjectView const* projectView() const noexcept;

    public slots:

        void onNewProject();

        void onOpenProject();

        void onCloseProject();

        void onSaveProject();

        void onOpenSettings();

        void onOpenFormats();

    signals:
        void projectAboutToChange(ProjectModel* model);

        void projectChanged(ProjectModel* model);

        void languageChanged();

    protected:
        void changeEvent(QEvent* event) override;

        void closeEvent(QCloseEvent* event) override;

        void showEvent(QShowEvent* event) override;

    private:
        std::unique_ptr<Ui::MainWindow> m_ui;
        std::unique_ptr<ProjectModel> m_model;
        DelegateAction m_undoAction{"Undo"};
        DelegateAction m_redoAction{"Redo"};

        bool continueCheckUnsavedChanges() const;

        QString generateWelcomeMessage() const;

    private slots:

        void onProjectAboutToChange(ProjectModel* m);

        void onProjectChanged(ProjectModel* m);

        void onItemAboutToRemoved(QModelIndex const& idx, ProjectModel::NodeType type);

        void onProjectViewFocus(bool focus);

        void onSceneTabFocus(bool focus);

        void onAboutToShowInspectionMenu();
    };
}

#endif //NOVELIST_MAINWINDOW_H
