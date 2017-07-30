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
#include "model/ProjectModel.h"

namespace novelist {

    namespace internal
    {
        class ProjectTreeView;
    }

    class ProjectView : public QWidget {
    Q_OBJECT

    public:
        explicit ProjectView(QWidget* parent);

        void retranslateUi();

        void setModel(ProjectModel* model);

        ProjectModel* model() const;

    protected slots:

        void onNewChapter();

        void onNewScene();

        void onRemoveEntry();

        void onProperties();

        void onDoubleClick(QModelIndex idx);

        void onSelectionChanged(QItemSelection const& selected, QItemSelection const& deselected);

        void onContextMenuEvent(QPoint const& pos);

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
        QMenu* m_contextMenu;

        void setup();

        void setupActions();

        void setupConnections();
    };

    namespace internal
    {
        class ProjectTreeView : public QTreeView
        {
            Q_OBJECT

        public:
            using QTreeView::QTreeView;

        protected:
            /**
             * @note The default implementation assumes that internal item movement is done by copying the source to
             *       destination, then deleting source. This is not possible with ProjectModels, because its items are
             *       not copyable. The ProjectModel implementation internally calls the move operation on drop, such
             *       that the remove operation of QTreeView is destructive.
             * @param supportedActions
             */
            void startDrag(Qt::DropActions supportedActions) override;
        };
    }
}

#endif //NOVELIST_PROJECTMODELVIEW_H
