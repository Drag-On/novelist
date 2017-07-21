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
#include "model/ProjectModel.h"

namespace novelist {
    class ProjectView : public QWidget {
    Q_OBJECT

    public:
        ProjectView(QWidget* parent);

        void retranslateUi();

    private:
        QVBoxLayout* m_topLayout;
        QHBoxLayout* m_nestedLayout;
        QTreeView* m_treeView;
        QToolButton* m_newSceneButton;
        QToolButton* m_newChapterButton;
        QToolButton* m_deleteButton;
        QSpacerItem* m_buttonSpacer;

        void setup();
    };
}

#endif //NOVELIST_PROJECTMODELVIEW_H
