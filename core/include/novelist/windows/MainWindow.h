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

        void onCanRedoChanged(bool canRedo);

        void onCanUndoChanged(bool canUndo);

        void onRedoTextChanged(QString const& redoText);

        void onUndoTextChanged(QString const& undoText);

        void onRedoSourceChanged(std::function<void()> redo);

        void onUndoSourceChanged(std::function<void()> undo);

    protected:
        void changeEvent(QEvent* event) override;

        void closeEvent(QCloseEvent* event) override;

    private:
        std::unique_ptr<Ui::MainWindow> m_ui;
        std::unique_ptr<ProjectModel> m_model;
        ConnectionWrapper m_undoTextChangedConnection;
        ConnectionWrapper m_redoTextChangedConnection;
        ConnectionWrapper m_canUndoChangedConnection;
        ConnectionWrapper m_canRedoChangedConnection;
        ConnectionWrapper m_undoSourceChangedConnection;
        ConnectionWrapper m_redoSourceChangedConnection;

        bool continueCheckUnsavedChanges() const;

    private slots:

        void onProjectChanged(ProjectModel* m);

        void onItemAboutToRemoved(QModelIndex const& idx, ProjectModel::NodeType type);

        void onProjectViewFocus(bool focus);

        void onSceneTabFocus(bool focus);

        void onSceneTabChange(int index);

        QString undoSceneModText() const;
    };
}

#endif //NOVELIST_MAINWINDOW_H
