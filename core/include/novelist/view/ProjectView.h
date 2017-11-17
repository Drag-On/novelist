/**********************************************************
 * @file   ProjectModelView.h
 * @author jan
 * @date   7/21/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_PROJECTMODELVIEW_H
#define NOVELIST_PROJECTMODELVIEW_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QtWidgets/QDialog>
#include "model/ProjectModel.h"

namespace novelist {

    namespace internal {
        class ProjectTreeView;
    }

    /**
     * Specialized tree view for projects
     */
    class ProjectView : public QWidget {
    Q_OBJECT

    public:
        /**
         * @param parent Parent widget
         */
        explicit ProjectView(QWidget* parent);

        /**
         * Translate UI to currently set language
         */
        void retranslateUi();

        /**
         * @param model New project model
         */
        void setModel(ProjectModel* model);

        /**
         * @return Currently set model or nullptr if none
         */
        ProjectModel* model() const;

        /**
         * Shows the project properties dialog. If the current model is nullptr, does nothing.
         * @return Dialog code
         */
        QDialog::DialogCode showProjectPropertiesDialog();

        /**
         * @return true if the project can be undone, otherwise false
         */
        QAction* undoAction() const;

        /**
         * @return true if the project can be redone, otherwise false
         */
        QAction* redoAction() const;

    signals:
        /**
         * Fires when the user requests to open a scene, e.g. via double click
         * @param idx
         */
        void openSceneRequested(QModelIndex idx);

        /**
         * Fires when the underlying model was switched out
         * @param m New model
         */
        void modelChanged(ProjectModel* m);

        /**
         * Fires when the view received focus or lost it
         * @param focused True when focus was gained, otherwise false
         */
        void focusReceived(bool focused);

    protected slots:
        /**
         * Creates a new chapter node based on the current selection
         */
        void onNewChapter();

        /**
         * Creates a new scene node based on the current selection
         */
        void onNewScene();

        /**
         * Removes the currently selected node
         */
        void onRemoveEntry();

        /**
         * Shows the properties of the currently selected node
         */
        void onProperties();

        /**
         * Handles behavior when a node is double-clicked. Emits open scene signal if the clicked node was a scene.
         * @param idx Index of the clicked node
         */
        void onDoubleClick(QModelIndex idx);

        /**
         * Enables or disables menu items based on selection
         * @param selected Selected items
         * @param deselected Deselected items
         */
        void onSelectionChanged(QItemSelection const& selected, QItemSelection const& deselected);

        /**
         * Shows the context menu
         * @param pos Position to show the menu at
         */
        void onContextMenuEvent(QPoint const& pos);

    protected:
        void focusInEvent(QFocusEvent* event) override;

        void focusOutEvent(QFocusEvent* event) override;

        void paintEvent(QPaintEvent* event) override;

    private:
        QIcon m_iconNewScene;
        QIcon m_iconNewChapter;
        QIcon m_iconRemove;
        QIcon m_iconConfig;
        QVBoxLayout* m_topLayout;
        QHBoxLayout* m_nestedLayout;
        internal::ProjectTreeView* m_treeView;
        QToolButton* m_newSceneButton;
        QToolButton* m_newChapterButton;
        QToolButton* m_deleteButton;
        QToolButton* m_propertiesButton;
        QSpacerItem* m_buttonSpacer;
        QAction* m_actionNewChapter;
        QAction* m_actionNewScene;
        QAction* m_actionRemoveEntry;
        QAction* m_actionProperties;
        QAction* m_actionUndo;
        QAction* m_actionRedo;
        QMenu* m_contextMenu;
        QColor const m_focusColor{QColor::fromRgb(196, 205, 221)};

        void setup();

        void setupActions();

        void setupConnections();
    };

    namespace internal {
        class ProjectTreeView : public QTreeView {
        Q_OBJECT

        public:
            explicit ProjectTreeView(ProjectView* parent) noexcept;

        protected:
            /**
             * @note The default implementation assumes that internal item movement is done by copying the source to
             *       destination, then deleting source. This is not possible with ProjectModels, because its items are
             *       not copyable. The ProjectModel implementation internally calls the move operation on drop, such
             *       that the remove operation of QTreeView is destructive.
             * @param supportedActions
             */
            void startDrag(Qt::DropActions supportedActions) override;

            void focusInEvent(QFocusEvent* event) override;

            void focusOutEvent(QFocusEvent* event) override;
        };
    }
}

#endif //NOVELIST_PROJECTMODELVIEW_H
