/**********************************************************
 * @file   InsightModel.cpp
 * @author jan
 * @date   10/18/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "inspection/InsightModel.h"
#include <QtCore/QCoreApplication>

namespace novelist {

    namespace internal {
        RemoveInsightEvent::RemoveInsightEvent(IInsight* insight)
                :QEvent(s_eventId),
                 m_insight(insight)
        {
        }
    }

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
                    return QVariant::fromValue<QString>(insightTypeToDescription(iter->get()->type()));
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
        auto insight_ptr = std::unique_ptr<IInsight>(insight);
        connect(dynamic_cast<QObject*>(insight_ptr.get()), SIGNAL(collapsed(IInsight * )),
                this, SLOT(onInsightCollapsed(IInsight * )));

        // Currently, insights can not change order. Once they can, this needs to be uncommented:
        // std::sort(m_insights.begin(), m_insights.end(), internal::InsightPtrComp());
        auto iter = findInsertLocation(insight_ptr);
        auto idx = gsl::narrow<int>(std::distance(m_insights.begin(), iter));

        beginInsertRows(QModelIndex(), idx, idx);
        m_insights.insert(iter, std::move(insight_ptr));
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

        emit insightRemoved();
        return true;
    }

    void InsightModel::clear()
    {
        beginResetModel();
        m_insights.clear();
        endResetModel();

        emit insightRemoved();
    }

    QModelIndex InsightModel::find(int charpos) const noexcept
    {
        auto iter = std::find_if(m_insights.begin(), m_insights.end(), [charpos](std::unique_ptr<IInsight> const& p) {
            return p->range().first <= charpos && p->range().second > charpos;
        });
        if (iter != m_insights.end())
            return index(gsl::narrow<int>(std::distance(m_insights.begin(), iter)), 0);
        return QModelIndex();
    }

    bool InsightModel::event(QEvent* event)
    {
        auto* e = dynamic_cast<internal::RemoveInsightEvent*>(event);
        if (e) {
            findAndRemove(e->m_insight);
            e->accept();
        }
        return QObject::event(event);
    }

    auto InsightModel::findInsertLocation(std::unique_ptr<IInsight> const& ptr) -> std::vector<InsightPtr>::iterator
    {
        internal::InsightPtrComp comp;
        auto iter = m_insights.begin();
        for (; iter != m_insights.end(); ++iter) {
            if (!comp(*iter, ptr))
                break;
        }
        return iter;
    }

    void InsightModel::findAndRemove(IInsight* insight)
    {
        auto iter = std::find_if(m_insights.begin(), m_insights.end(), [insight](std::unique_ptr<IInsight> const& p) {
            return p.get() == insight;
        });
        if (iter != m_insights.end()) {
            remove(index(gsl::narrow<int>(std::distance(m_insights.begin(), iter)), 0));
            emit insightRemoved();
        }
    }

    void InsightModel::onInsightCollapsed(IInsight* insight)
    {
        // Note: Can't directly remove here, because it would delete the caller. Therefore post event that will then
        // delete the correct element on next event update (see event(QEvent*)).
        QCoreApplication::postEvent(this, new internal::RemoveInsightEvent(insight));
    }
}