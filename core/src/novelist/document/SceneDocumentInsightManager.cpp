/**********************************************************
 * @file   SceneDocumentInsightManager.cpp
 * @author jan
 * @date   10/29/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <gsl/gsl>
#include <QtCore/QCoreApplication>
#include "document/SceneDocumentInsightManager.h"
#include "document/SceneDocument.h"

namespace novelist {
    namespace internal {
        RemoveInsightEvent::RemoveInsightEvent(Insight const* insight)
                :QEvent(s_eventId),
                 m_insight(insight)
        {
        }
    }

    int SceneDocumentInsightManager::insert(std::unique_ptr<Insight> insight)
    {
        auto iter = m_insights.insert(std::move(insight));
        rehighlight(iter->get());
        return gsl::narrow_cast<int>(std::distance(m_insights.begin(), iter));
    }

    int SceneDocumentInsightManager::insert(std::unique_ptr<Insight> insight, SVector::const_iterator hint)
    {
        auto iter = m_insights.insert(std::move(insight), hint);
        rehighlight(iter->get());
        return gsl::narrow_cast<int>(std::distance(m_insights.begin(), iter));
    }

    auto SceneDocumentInsightManager::begin() const noexcept -> SVector::const_iterator
    {
        return m_insights.begin();
    }

    auto SceneDocumentInsightManager::end() const noexcept -> SVector::const_iterator
    {
        return m_insights.end();
    }

    auto SceneDocumentInsightManager::erase(SVector::const_iterator iter) noexcept -> SVector::const_iterator
    {
        auto parRange = novelist::parRange(**iter);
        auto afterIter = m_insights.erase(iter);
        rehighlight(parRange);
        return afterIter;
    }

    size_t SceneDocumentInsightManager::size() const noexcept
    {
        return m_insights.size();
    }

    void SceneDocumentInsightManager::clear() noexcept
    {
        m_insights.clear();
    }

    bool SceneDocumentInsightManager::event(QEvent* event)
    {
        auto* removeTextMarkerEvent = dynamic_cast<internal::RemoveInsightEvent*>(event);
        if (removeTextMarkerEvent) {
            findAndAutoRemove(removeTextMarkerEvent->m_insight);
            removeTextMarkerEvent->accept();
        }
        return QObject::event(event);
    }

    void SceneDocumentInsightManager::highlightBlock(QString const&)
    {
        // Block state stores an index into the insights vector, indicating the first insight that might be relevant
        // for the next block. This information is updated every time a block is highlighted.
        auto thisBlockState = previousBlockState() >= 0 ? previousBlockState() : 0;
        bool blockStateChanged = false;
        bool foundValidInsight = false;
        int blockNum = currentBlock().blockNumber();
        int blockPos = currentBlock().position();
        auto coversCurBlock = [this, blockNum](int i) {
            return parRange(*m_insights[i]).first <= blockNum && parRange(*m_insights[i]).second >= blockNum;
        };
        for (int i = thisBlockState;
             i < gsl::narrow_cast<int>(m_insights.size()) && (!foundValidInsight || coversCurBlock(i));
             ++i) {

            auto const& m = m_insights[i];
            auto const& range = m->range();
            auto const& parRange = novelist::parRange(*m);

            // If insight has zero length, mark it for removal and continue
            if (empty(*m)) {
                QCoreApplication::postEvent(this, new internal::RemoveInsightEvent(m.get()));
                continue;
            }

            // Skip if marker doesn't cover this block
            if (!coversCurBlock(i))
                continue;
            else
                foundValidInsight = true;

            // Save the first item that spans into this paragraph
            if (parRange.second > blockNum && !blockStateChanged) {
                thisBlockState = i;
                blockStateChanged = true;
            }

            // Format range, but consider whether it spans multiple paragraphs
            int const start = [blockNum, blockPos, &parRange, &range] {
                if (parRange.first < blockNum)
                    return 0;
                return range.first - blockPos;
            }();
            int const end = [this, blockNum, blockPos, &parRange, &range] {
                if (parRange.second > blockNum)
                    return currentBlock().length();
                return range.second - blockPos;
            }();
            setFormat(start, end - start, m->format());
        }
        setCurrentBlockState(thisBlockState);
    }

    void SceneDocumentInsightManager::findAndAutoRemove(Insight const* insight)
    {
        auto iter = std::find_if(m_insights.begin(), m_insights.end(),
                [insight](std::unique_ptr<Insight> const& p) {
                    return p.get() == insight;
                });
        if (iter != m_insights.end()) {
            auto index = gsl::narrow_cast<int>(std::distance(m_insights.begin(), iter));
            emit aboutToAutoRemove(index);
            erase(iter);
            emit autoRemoved(index);
        }
    }

    typename SceneDocumentInsightManager::SVector::const_iterator
    findInsertIdx(SceneDocumentInsightManager const& mgr, std::unique_ptr<Insight> const& element)
    {
        return findInsertIdx(mgr.m_insights, element);
    }

    void SceneDocumentInsightManager::rehighlight(Insight const* insight)
    {
        auto parRange = novelist::parRange(*insight);
        rehighlight(parRange);
    }

    void SceneDocumentInsightManager::rehighlight(std::pair<int, int> const& parRange)
    {
        for (auto block = document()->findBlockByNumber(parRange.first);
             block.isValid() && (block.blockNumber() <= parRange.second);
             block = block.next()) {
            rehighlightBlock(block);
        }
    }
}