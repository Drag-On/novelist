/**********************************************************
 * @file   InsightModel.h
 * @author jan
 * @date   10/18/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_INSIGHTMODEL_H
#define NOVELIST_INSIGHTMODEL_H

#include <QtCore/QAbstractTableModel>
#include <QtCore/QEvent>
#include <gsl/gsl>
#include <memory>
#include <vector>
#include "util/ConnectionWrapper.h"
#include "document/SceneDocument.h"
#include "document/Insight.h"

namespace novelist {
    /**
     * Additional roles for InsightModel
     */
    enum class InsightModelRoles {
        InsightDataRole = Qt::UserRole //!< To get a pointer to the insight
    };

    class TextEditor;

    /**
     * Can be used to display all insights of a TextEditor's underlying SceneDocument in a table view
     */
    class InsightModel : public QAbstractTableModel {
    Q_OBJECT

    public:
        explicit InsightModel(SceneDocument* doc);

        int rowCount(QModelIndex const& parent = QModelIndex{}) const override;

        int columnCount(QModelIndex const& parent = QModelIndex{}) const override;

        QVariant data(const QModelIndex& index, int role) const override;

        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

        void setDocument(SceneDocument* doc);

        /**
         * Insert an insight
         * @param insight New insight
         * @return Index of the newly inserted element
         */
        QModelIndex insert(std::unique_ptr<Insight> insight);

        /**
         * Remove element at \p index
         * @param index Index of element to remove
         * @return True in case of success, otherwise false
         */
        bool remove(QModelIndex const& index);

        /**
         * Remove all non-persistent elements in a certain range
         * @param start Start of the range
         * @param end End of the range
         * @return True in case elements have been erased, otherwise false
         */
        bool removeNonPersistentInRange(int start, int end);

        /**
         * Remove all elements
         */
        void clear();

        /**
         * Finds the first insight that contains a given character position.
         * @param charpos Character position in the document
         * @return Index of the first insight that contains \p charpos or an invalid index.
         */
        QModelIndex find(int charpos) const noexcept;

    private:
        SceneDocument* m_doc = nullptr;
        ConnectionWrapper m_aboutToAutoRemoveConnection;
        ConnectionWrapper m_autoRemovedConnection;

        SceneDocumentInsightManager* insightManager();
        SceneDocumentInsightManager const* insightManager() const;
        void onAboutToAutoRemove(int idx);
        void onAutoRemoved(int idx);
    };
}

#endif //NOVELIST_INSIGHTMODEL_H
