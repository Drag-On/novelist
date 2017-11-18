/**********************************************************
 * @file   InsightModel.cpp
 * @author jan
 * @date   10/18/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "widgets/texteditor/InsightModel.h"
#include "widgets/texteditor/TextEditor.h"
#include <QtCore/QCoreApplication>

namespace novelist {

    InsightModel::InsightModel(SceneDocument* doc)
    {
        setDocument(doc);
    }

    int InsightModel::rowCount(QModelIndex const& /*parent*/) const
    {
        if (insightManager())
            return gsl::narrow<int>(insightManager()->size());
        return 0;
    }

    int InsightModel::columnCount(QModelIndex const& /*parent*/) const
    {
        return 4;
    }

    QVariant InsightModel::data(const QModelIndex& index, int role) const
    {
        if (!insightManager())
            return QVariant();

        if (role == Qt::DisplayRole || role == Qt::ToolTipRole) {
            auto iter = insightManager()->begin();
            std::advance(iter, index.row());
            auto* insight = dynamic_cast<Insight*>(iter->get());
            if (!insight)
                return QVariant();
            switch (index.column()) {
                case 0:
                    return QVariant::fromValue<int>(parRange(*insight).first + 1);
                case 1:
                    return QVariant::fromValue<int>(parRange(*insight).second + 1);
                case 2:
                    return QVariant::fromValue<QString>(insight->category());
                case 3:
                    return QVariant::fromValue<QString>(insight->message());
                default:
                    return QVariant();
            }
        }
        else if (role == static_cast<int>(InsightModelRoles::InsightDataRole)) {
            auto iter = insightManager()->begin();
            std::advance(iter, index.row());
            return QVariant::fromValue<Insight*>(dynamic_cast<Insight*>(iter->get()));
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

    void InsightModel::setDocument(SceneDocument* doc)
    {
        m_doc = doc;
        if (m_doc) {
            m_aboutToAutoRemoveConnection = connect(
                    &m_doc->insightManager(), &SceneDocumentInsightManager::aboutToAutoRemove,
                    this, &InsightModel::onAboutToAutoRemove);
            m_autoRemovedConnection = connect(
                    &m_doc->insightManager(), &SceneDocumentInsightManager::autoRemoved,
                    this, &InsightModel::onAutoRemoved);
        }
        else {
            m_aboutToAutoRemoveConnection.disconnect();
            m_autoRemovedConnection.disconnect();
        }
    }

    QModelIndex InsightModel::insert(std::unique_ptr<Insight> insight)
    {
        if (!insightManager())
            return QModelIndex();

        auto iter = findInsertIdx(*insightManager(), insight);
        auto idx = gsl::narrow<int>(std::distance(insightManager()->begin(), iter));

        beginInsertRows(QModelIndex(), idx, idx);
        insightManager()->insert(std::move(insight), iter);
        endInsertRows();
        return index(idx, 0);
    }

    bool InsightModel::remove(QModelIndex const& index)
    {
        if (!insightManager())
            return false;

        if (!index.isValid())
            return false;

        if (gsl::narrow<int>(insightManager()->size()) <= index.row())
            return false;

        auto iter = insightManager()->begin();
        std::advance(iter, index.row());
        beginRemoveRows(index.parent(), index.row(), index.row());
        insightManager()->erase(iter);
        endRemoveRows();

        return true;
    }

    bool InsightModel::removeNonPersistentInRange(int start, int end)
    {
        if (!insightManager())
            return false;

        bool removed = false;
        auto overlap = [start, end](Insight* a) {
            return a->range().first < end && a->range().second > start;
        };
        for (auto iter = insightManager()->begin(); iter != insightManager()->end();) {
            if (!(*iter)->isPersistent() && overlap(iter->get())) {
                auto idx = gsl::narrow<int>(std::distance(insightManager()->begin(), iter));
                beginRemoveRows(QModelIndex(), idx, idx);
                iter = insightManager()->erase(iter);
                endRemoveRows();
                removed = true;
            }
            else
                ++iter;
        }
        return removed;
    }

    void InsightModel::clear()
    {
        if (!insightManager())
            return;

        beginResetModel();
        insightManager()->clear();
        endResetModel();
    }

    QModelIndex InsightModel::find(int charpos) const noexcept
    {
        if (!insightManager())
            return QModelIndex();

        auto iter = std::find_if(insightManager()->begin(), insightManager()->end(),
                [charpos](std::unique_ptr<Insight> const& p) {
                    return p->range().first <= charpos && p->range().second > charpos;
                });
        if (iter != insightManager()->end())
            return index(gsl::narrow<int>(std::distance(insightManager()->begin(), iter)), 0);
        return QModelIndex();
    }

    SceneDocumentInsightManager* InsightModel::insightManager()
    {
        if (m_doc)
            return &m_doc->insightManager();
        return nullptr;
    }

    SceneDocumentInsightManager const* InsightModel::insightManager() const
    {
        if (m_doc)
            return &m_doc->insightManager();
        return nullptr;
    }

    void InsightModel::onAboutToAutoRemove(int idx)
    {
        beginRemoveRows(QModelIndex(), idx, idx);
    }

    void InsightModel::onAutoRemoved(int /*idx*/)
    {
        endRemoveRows();
    }
}