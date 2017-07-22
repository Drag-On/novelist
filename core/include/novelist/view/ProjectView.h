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
        QTreeView* m_treeView;
        QToolButton* m_newSceneButton;
        QToolButton* m_newChapterButton;
        QToolButton* m_deleteButton;
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
}

#endif //NOVELIST_PROJECTMODELVIEW_H
