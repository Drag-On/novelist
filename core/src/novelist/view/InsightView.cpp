/**********************************************************
 * @file   InsightView.cpp
 * @author jan
 * @date   10/22/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "view/InsightView.h"
#include "widgets/SceneTabWidget.h"

namespace novelist {
    InsightView::InsightView(QWidget* parent)
            :QTableView(parent)
    {
    }

    void InsightView::useSceneTabWidget(SceneTabWidget* sceneTabs)
    {
        m_sceneTabs = sceneTabs;
        if (m_sceneTabs)
            m_doubleClickedConnectionWrapper = connect(this, &InsightView::doubleClicked, this,
                    &InsightView::onDoubleClicked);
        else
            m_doubleClickedConnectionWrapper.disconnect();
    }

    void InsightView::setModel(QAbstractItemModel* model)
    {
        auto* m = dynamic_cast<InsightModel*>(model);
        if(m || model == nullptr)
            setModel(m);
        else
            throw std::invalid_argument("Model isn't an InsightModel.");
    }

    void InsightView::setModel(InsightModel* model)
    {
        QTableView::setModel(model);
    }

    InsightModel* InsightView::model() const
    {
        return dynamic_cast<InsightModel*>(QAbstractItemView::model());
    }

    void InsightView::onDoubleClicked(QModelIndex const& index)
    {
        Insight* insight = qvariant_cast<Insight*>(model()->data(index, static_cast<int>(InsightModelRoles::InsightDataRole)));
        auto* editor = dynamic_cast<TextEditor*>(m_sceneTabs->currentWidget());
        if(editor) {
            auto cursor = editor->textCursor();
            cursor.setPosition(insight->range().second);
            cursor.setPosition(insight->range().first, QTextCursor::KeepAnchor);
            editor->setTextCursor(cursor);
            editor->setFocus();
        }
    }
}