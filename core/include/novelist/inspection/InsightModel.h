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
#include "IInsight.h"

namespace novelist {
    namespace internal {
        struct InsightPtrComp {
            bool operator()(std::unique_ptr<IInsight> const& a, std::unique_ptr<IInsight> const& b) const noexcept
            {
                return a->range().first < b->range().first ||
                        (a->range().first == b->range().first && a->range().second < b->range().second);
            }
        };

        class RemoveInsightEvent : public QEvent {
        public:
            explicit RemoveInsightEvent(IInsight* insight);
            IInsight* m_insight;
            static inline QEvent::Type const s_eventId = static_cast<QEvent::Type>(QEvent::registerEventType());
        };
    }

    /**
     * Additional roles for InsightModel
     */
    enum class InsightModelRoles {
        DataRole = Qt::UserRole //!< To get a pointer to the insight
    };

    /**
     * Contains insights that can be displayed in a table view
     */
    class InsightModel : public QAbstractTableModel {
    Q_OBJECT

    public:
        int rowCount(QModelIndex const& parent = QModelIndex{}) const override;

        int columnCount(QModelIndex const& parent = QModelIndex{}) const override;

        QVariant data(const QModelIndex& index, int role) const override;

        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

        /**
         * Insert an insight. The model claims ownership over it.
         * @param insight New insight
         * @return Index of the newly inserted element
         */
        QModelIndex insert(gsl::owner<IInsight*> insight);

        /**
         * Remove element at \p index
         * @param index Index of element to remove
         * @return True in case of success, otherwise false
         */
        bool remove(QModelIndex const& index);

        /**
         * Remove all elements
         */
        void clear();

        bool event(QEvent* event) override;

    private:
        using InsightPtr = std::unique_ptr<IInsight>;
        std::vector<InsightPtr> m_insights{};

        std::vector<InsightPtr>::iterator findInsertLocation(std::unique_ptr<IInsight> const& ptr);

        void findAndRemove(IInsight* insight);

    private slots:
        void onInsightCollapsed(IInsight* insight);
    };
}

#endif //NOVELIST_INSIGHTMODEL_H
