/**********************************************************
 * @file   SceneTabWidget.cpp
 * @author jan
 * @date   8/6/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <QTabBar>
#include <QMessageBox>
#include "widgets/SceneTabWidget.h"

namespace novelist {
    SceneTabWidget::SceneTabWidget(QWidget* parent)
            :QTabWidget(parent)
    {
        setDocumentMode(true);
        setTabsClosable(true);
        setMovable(true);
        setElideMode(Qt::ElideRight);

        connect(this, &SceneTabWidget::tabCloseRequested, this, &SceneTabWidget::onTabCloseRequested);
    }

    void SceneTabWidget::openScene(ProjectModel* model, QModelIndex index)
    {
        Expects(model->nodeType(index) == ProjectModel::NodeType::Scene);

        int tabIdx = indexOf(model, index);
        if (tabIdx == -1) {
            auto* editor = new internal::InternalTextEditor(); // This has to be deleted on tab removal, see closeScene()
            editor->m_model = model;
            editor->m_modelIndex = index;
            editor->setDocument(model->loadScene(index));

            // Make sure the tab title and color change appropriately
            auto onDataChanged =
                    [this, editor, model](QModelIndex const& topLeft, QModelIndex const& bottomRight) {
                        for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {
                            QModelIndex curIdx = model->index(i, 0, topLeft.parent());
                            if (int tabIdx = indexOf(model, curIdx); tabIdx >= 0) {
                                QString name = model->data(curIdx, Qt::DisplayRole).toString();
                                tabBar()->setTabText(tabIdx, name);
                                tabBar()->setTabToolTip(tabIdx, name);
                                tabBar()->setTabTextColor(tabIdx,
                                        qvariant_cast<QBrush>(model->data(curIdx, Qt::ForegroundRole)).color());
                            }
                        }
                    };
            connect(editor->m_model, &ProjectModel::dataChanged, onDataChanged);

            QString name = model->data(index, Qt::DisplayRole).toString();
            tabIdx = addTab(editor, name);
            tabBar()->setTabToolTip(tabIdx, name);
            tabBar()->setTabTextColor(tabIdx, qvariant_cast<QBrush>(model->data(index, Qt::ForegroundRole)).color());
        }
        setCurrentIndex(tabIdx);
    }

    void SceneTabWidget::closeScene(ProjectModel* model, QModelIndex index)
    {
        closeScene(indexOf(model, index));
    }

    void SceneTabWidget::closeScene(int index, bool userCheck)
    {
        if(index < 0)
            return;

        auto* w = dynamic_cast<internal::InternalTextEditor*>(widget(index));
        if (w != nullptr) {

            int action = QMessageBox::Discard;
            if(w->document() != nullptr && w->document()->isModified() && userCheck) {
                QMessageBox msgBox;
                msgBox.setText(tr("The document has been modified."));
                msgBox.setInformativeText(
                        tr("If you close it now, all changes will be lost. Are you sure you want to continue?"));
                msgBox.setStandardButtons(QMessageBox::Discard | QMessageBox::Cancel);
                msgBox.setDefaultButton(QMessageBox::Cancel);
                msgBox.setIcon(QMessageBox::Question);
                action = msgBox.exec();
            }

            if(action == QMessageBox::Discard)
            {
                removeTab(index);
                w->m_model->unloadScene(w->m_modelIndex);
                delete w;
            }
            return;
        }

        removeTab(index);
    }

    void SceneTabWidget::closeAll(bool userCheck)
    {
        for(int c = count(), i = 0; i < c; ++i)
            closeScene(0, userCheck);
    }

    int SceneTabWidget::indexOf(ProjectModel const* model, QModelIndex index) const
    {
        for (int i = 0; i < count(); ++i) {
            auto* w = dynamic_cast<internal::InternalTextEditor*>(widget(i));
            if (w != nullptr && w->m_model == model && w->m_modelIndex == index)
                return i;
        }

        return -1;
    }

    void SceneTabWidget::onTabCloseRequested(int index)
    {
        closeScene(index);
    }
}