/**********************************************************
 * @file   InsightModel.cpp
 * @author jan
 * @date   10/18/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "inspection/InsightModel.h"

namespace novelist {
    int InsightModel::rowCount(QModelIndex const& /*parent*/) const
    {
        return gsl::narrow<int>(m_insights.size());
    }

    int InsightModel::columnCount(QModelIndex const& /*parent*/) const
    {
        return 3;
    }

    QVariant InsightModel::data(const QModelIndex& index, int role) const
    {
        if (role == Qt::DisplayRole) {
            auto iter = m_insights.begin();
            std::advance(iter, index.row());
            switch (index.column()) {
                case 0:
                    return QVariant::fromValue<int>(iter->get()->range().first);
                case 1:
                    return QVariant::fromValue<int>(iter->get()->range().second);
                case 2:
                    return QVariant::fromValue<QString>(iter->get()->message());
                default:
                    return QVariant();
            }
        }
        else if (role == static_cast<int>(InsightModelRoles::DataRole)) {
            auto iter = m_insights.begin();
            std::advance(iter, index.row());
            return QVariant::fromValue<IInsight*>(iter->get());
        }
        return QVariant();
    }

    QVariant InsightModel::headerData(int section, Qt::Orientation /*orientation*/, int /*role*/) const
    {
        switch (section) {
            case 0:
                //: For "left"
                return tr("l");
            case 1:
                //: For "right"
                return tr("r");
            case 2:
                return tr("Message");
        }
        return QVariant();
    }

    QModelIndex InsightModel::insert(gsl::owner<IInsight*> insight)
    {
        auto result = m_insights.emplace(insight);
        return index(gsl::narrow<int>(std::distance(m_insights.begin(), result.first)), 0);
    }

    bool InsightModel::remove(QModelIndex const& index)
    {
        if (!index.isValid())
            return false;

        if (gsl::narrow<int>(m_insights.size()) <= index.row())
            return false;

        auto iter = m_insights.begin();
        std::advance(iter, index.row());
        m_insights.erase(iter);

        return true;
    }
}