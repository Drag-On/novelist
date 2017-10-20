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
        return 4;
    }

    QVariant InsightModel::data(const QModelIndex& index, int role) const
    {
        if (role == Qt::DisplayRole) {
            auto iter = m_insights.begin();
            std::advance(iter, index.row());
            switch (index.column()) {
                case 0:
                    return QVariant::fromValue<int>(iter->get()->parRange().first + 1);
                case 1:
                    return QVariant::fromValue<int>(iter->get()->parRange().second + 1);
                case 2:
                    return QVariant::fromValue<QString>(iter->get()->type());
                case 3:
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

    QVariant InsightModel::headerData(int section, Qt::Orientation orientation, int role) const
    {
        if (role != Qt::DisplayRole)
            return QVariant();

        if (orientation == Qt::Orientation::Horizontal) {
            switch (section) {
                case 0:
                    return tr("First");
                case 1:
                    return tr("Last");
                case 2:
                    return tr("Type");
                case 3:
                    return tr("Message");
            }
        }
        return QVariant();
    }

    QModelIndex InsightModel::insert(gsl::owner<IInsight*> insight)
    {
        auto result = m_insights.emplace(insight);
        auto idx = gsl::narrow<int>(std::distance(m_insights.begin(), result.first));
        beginInsertRows(QModelIndex(), idx, idx);
        endInsertRows();
        return index(idx, 0);
    }

    bool InsightModel::remove(QModelIndex const& index)
    {
        if (!index.isValid())
            return false;

        if (gsl::narrow<int>(m_insights.size()) <= index.row())
            return false;

        auto iter = m_insights.begin();
        std::advance(iter, index.row());
        beginRemoveRows(index.parent(), index.row(), index.row());
        m_insights.erase(iter);
        endRemoveRows();

        return true;
    }

    void InsightModel::clear()
    {
        beginResetModel();
        m_insights.clear();
        endResetModel();
    }
}