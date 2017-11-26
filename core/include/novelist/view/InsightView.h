/**********************************************************
 * @file   InsightView.h
 * @author jan
 * @date   10/22/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_INSIGHTVIEW_H
#define NOVELIST_INSIGHTVIEW_H

#include <QtWidgets/QTableView>
#include <gsl/gsl>
#include "widgets/texteditor/TextEditor.h"
#include <novelist_core_export.h>

namespace novelist {
    class SceneTabWidget;

    /**
     * A specialized table view for showing all insights of the currently active TextEditor in a SceneTabWidget. Double
     * clicking an entry will take the TextEditor's cursor to the selected insight.
     */
    class NOVELIST_CORE_EXPORT InsightView : public QTableView {
        Q_OBJECT

    public:
        explicit InsightView(QWidget* parent = nullptr);

        /**
         * @param sceneTabs Use this SceneTabWidget to find the appropriate TextEditor
         */
        void useSceneTabWidget(SceneTabWidget* sceneTabs);

        void setModel(QAbstractItemModel* model) override;

        void setModel(InsightModel* model);

        InsightModel* model() const;

    protected:
        void changeEvent(QEvent* event) override;

    private:
        SceneTabWidget* m_sceneTabs = nullptr;
        ConnectionWrapper m_doubleClickedConnectionWrapper;

        void onDoubleClicked(QModelIndex const& index);
    };
}

#endif //NOVELIST_INSIGHTVIEW_H
