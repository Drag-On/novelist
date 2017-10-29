/**********************************************************
 * @file   Insight.cpp
 * @author jan
 * @date   10/28/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include "document/Insight.h"
#include "document/SceneDocument.h"

namespace novelist {
    std::pair<int, int> parRange(Insight const& insight) noexcept
    {
        auto range = insight.range();
        int firstBlockNum = insight.document()->findBlock(range.first).blockNumber();
        int lastBlockNum = insight.document()->findBlock(range.second).blockNumber();
        return std::make_pair(firstBlockNum, lastBlockNum);
    }

    bool empty(Insight const& insight) noexcept
    {
        return length(insight) == 0;
    }

    int length(Insight const& insight) noexcept
    {
        auto range = insight.range();
        return range.second - range.first;
    }
}
