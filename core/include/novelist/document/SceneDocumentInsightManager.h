/**********************************************************
 * @file   SceneDocumentInsightManager.h
 * @author jan
 * @date   10/29/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_SCENEDOCUMENTINSIGHTMANAGER_H
#define NOVELIST_SCENEDOCUMENTINSIGHTMANAGER_H

#include <memory>
#include <QtCore/QEvent>
#include <QtGui/QSyntaxHighlighter>
#include "datastructures/SortedVector.h"
#include "Insight.h"

namespace novelist {
    namespace internal {
        class InsightPtrOrderCompare : private InsightOrderCompare {
        public:
            bool operator() (std::unique_ptr<Insight> const& lhs, std::unique_ptr<Insight> const& rhs) {
                if (lhs && rhs)
                    return InsightOrderCompare::operator()(*lhs, *rhs);
                else
                    return lhs && !rhs;
            }
        };

        class RemoveInsightEvent : public QEvent {
        public:
            explicit RemoveInsightEvent(Insight const* insight);
            Insight const* m_insight;
            static inline QEvent::Type const s_eventId = static_cast<QEvent::Type>(QEvent::registerEventType());
        };
    }

    /**
     * Manages markers on a document, i.e. makes sure that they are removed when they collapse to zero length and it
     * also makes sure they are properly rendered.
     */
    class SceneDocumentInsightManager : public QSyntaxHighlighter {
        Q_OBJECT

    public:
        using SVector = SortedVector<std::unique_ptr<Insight>, internal::InsightPtrOrderCompare>;
        using QSyntaxHighlighter::QSyntaxHighlighter;

        /**
         * Insert a new insight
         * @param insight Insight to insert
         * @return Index of the inserted insight
         */
        int insert(std::unique_ptr<Insight> insight);

        /**
         * Insert a new insight
         * @param insight Insight to insert
         * @param hint Hint to the position to insert at
         * @return Index of the inserted insight
         */
        int insert(std::unique_ptr<Insight> insight, SVector::const_iterator hint);

        /**
         * @return Iterator to first element
         */
        SVector::const_iterator begin() const noexcept;

        /**
         * @return Iterator past the last element
         */
        SVector::const_iterator end() const noexcept;

        /**
         * Erases an insight from this manager
         * @param iter Iterator pointing at the element to erase
         * @return Iterator to the element after the erased element
         */
        SVector::const_iterator erase(SVector::const_iterator iter) noexcept;

        /**
         * @return Amount of insights managed
         */
        size_t size() const noexcept;

        /**
         * Remove all insights at once
         */
        void clear() noexcept;

        bool event(QEvent* event) override;

    signals:
        /**
         * Fired when the insight at position \p index is about to be auto-removed due to collapsing to zero-length
         * @param index Index of the insight to be removed
         */
        void aboutToAutoRemove(int index);

        /**
         * Fired right after the insight at position \p index was auto-removed due to collapsing to zero-length
         * @param index Index of the insight that was removed. Note that this index is not valid anymore.
         */
        void autoRemoved(int index);

    protected:
        void highlightBlock(const QString& /*text*/) override;

    private:
        SVector m_insights{};

        void findAndAutoRemove(Insight const* insight);

        friend typename SceneDocumentInsightManager::SVector::const_iterator findInsertIdx(SceneDocumentInsightManager const& mgr,
                std::unique_ptr<Insight> const& element);

        void rehighlight(Insight const* insight);

        void rehighlight(std::pair<int, int> const& parRange);
    };
}

#endif //NOVELIST_SCENEDOCUMENTINSIGHTMANAGER_H
