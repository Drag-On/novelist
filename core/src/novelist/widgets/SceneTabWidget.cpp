/**********************************************************
 * @file   SceneTabWidget.cpp
 * @author jan
 * @date   8/6/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <QTabBar>
#include "widgets/SceneTabWidget.h"

namespace novelist {
    SceneTabWidget::SceneTabWidget(QWidget* parent)
            :QTabWidget(parent)
    {
        setTabBar(new internal::InternalTabBar(this));
        setDocumentMode(true);
        setTabsClosable(true);
        setMovable(true);
        setElideMode(Qt::ElideRight);

        m_boldAction.setCheckable(true);
        m_italicAction.setCheckable(true);
        m_underlineAction.setCheckable(true);
        m_overlineAction.setCheckable(true);
        m_strikethroughAction.setCheckable(true);
        m_smallCapsAction.setCheckable(true);

        connect(this, &SceneTabWidget::tabCloseRequested, this, &SceneTabWidget::onTabCloseRequested);
        connect(this, &SceneTabWidget::currentChanged, this, &SceneTabWidget::onCurrentChanged);
    }

    void SceneTabWidget::openScene(ProjectModel* model, QModelIndex index)
    {
        Expects(model->nodeType(index) == ProjectModel::NodeType::Scene);

        int tabIdx = indexOf(model, index);
        if (tabIdx == -1) {
            auto* editor = new internal::InternalTextEditor(); // The tab will take ownership
            editor->setFocusPolicy(focusPolicy());
            editor->m_model = model;
            editor->m_modelIndex = index;
            editor->setDocument(qvariant_cast<SceneDocument*>(model->data(index, ProjectModel::DocumentRole)));

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
            connect(editor, &TextEditor::focusReceived, [this] (bool focus){ emit focusReceived(focus); });

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

    void SceneTabWidget::closeScene(int index)
    {
        if(index < 0)
            return;

        removeTab(index);
    }

    void SceneTabWidget::closeAll()
    {
        for(int c = count(), i = 0; i < c; ++i)
            closeScene(0);
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

    QAction* SceneTabWidget::undoAction()
    {
        return &m_undoAction;
    }

    QAction* SceneTabWidget::redoAction()
    {
        return &m_redoAction;
    }

    QAction* SceneTabWidget::boldAction()
    {
        return &m_boldAction;
    }

    QAction* SceneTabWidget::italicAction()
    {
        return &m_italicAction;
    }

    QAction* SceneTabWidget::underlineAction()
    {
        return &m_underlineAction;
    }

    QAction* SceneTabWidget::overlineAction()
    {
        return &m_overlineAction;
    }

    QAction* SceneTabWidget::strikethroughAction()
    {
        return &m_strikethroughAction;
    }

    QAction* SceneTabWidget::smallCapsAction()
    {
        return &m_smallCapsAction;
    }

    void SceneTabWidget::focusInEvent(QFocusEvent* event)
    {
        emit focusReceived(true);
        onCurrentChanged(currentIndex());

        QWidget::focusInEvent(event);
    }

    void SceneTabWidget::focusOutEvent(QFocusEvent* event)
    {
        emit focusReceived(false);

        QWidget::focusOutEvent(event);
    }

    void SceneTabWidget::onTabCloseRequested(int index)
    {
        closeScene(index);
    }

    void SceneTabWidget::onCurrentChanged(int /*index*/)
    {
        auto* editor = dynamic_cast<internal::InternalTextEditor*>(currentWidget());
        if (editor) {
            QString title = editor->m_model->data(editor->m_modelIndex, Qt::DisplayRole).toString();
            m_undoAction.setDelegate(editor, &TextEditor::undo, &TextEditor::canUndo, &TextEditor::undoAvailable,
                    tr("Undo modification of \"%1\"").arg(title));
            m_redoAction.setDelegate(editor, &TextEditor::redo, &TextEditor::canRedo, &TextEditor::redoAvailable,
                    tr("Redo modification of \"%1\"").arg(title));

            m_boldAction.setEnabled(true);
            m_boldAction.setDelegate(editor->boldAction());
            m_italicAction.setEnabled(true);
            m_italicAction.setDelegate(editor->italicAction());
            m_underlineAction.setEnabled(true);
            m_underlineAction.setDelegate(editor->underlineAction());
            m_overlineAction.setEnabled(true);
            m_overlineAction.setDelegate(editor->overlineAction());
            m_strikethroughAction.setEnabled(true);
            m_strikethroughAction.setDelegate(editor->strikethroughAction());
            m_smallCapsAction.setEnabled(true);
            m_smallCapsAction.setDelegate(editor->smallCapsAction());
        }
        else {
            m_boldAction.setEnabled(false);
            m_italicAction.setEnabled(false);
            m_underlineAction.setEnabled(false);
            m_overlineAction.setEnabled(false);
            m_strikethroughAction.setEnabled(false);
            m_smallCapsAction.setEnabled(false);
        }
    }
}