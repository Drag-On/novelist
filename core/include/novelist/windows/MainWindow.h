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

namespace Ui {
    class MainWindow;
}

namespace novelist {
    class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        explicit MainWindow(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

        ~MainWindow() noexcept override;

        /**
         * Translate UI to new language
         */
        void retranslateUi();

    public slots:

        void onNewProject();

        void onOpenProject();

        void onCloseProject();

        void onSaveProject();

    protected:
        void changeEvent(QEvent* event) override;

        void closeEvent(QCloseEvent* event) override;

    private:
        std::unique_ptr<Ui::MainWindow> m_ui;
        std::unique_ptr<ProjectModel> m_model;
        DelegateAction m_undoAction{"Undo"};
        DelegateAction m_redoAction{"Redo"};

        bool continueCheckUnsavedChanges() const;

    private slots:

        void onProjectChanged(ProjectModel* m);

        void onItemAboutToRemoved(QModelIndex const& idx, ProjectModel::NodeType type);

        void onProjectViewFocus(bool focus);

        void onSceneTabFocus(bool focus);
    };
}

#endif //NOVELIST_MAINWINDOW_H
