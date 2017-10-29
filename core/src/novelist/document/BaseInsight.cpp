/**********************************************************
 * @file   BaseInsight.cpp
 * @author jan
 * @date   10/28/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtCore/QCoreApplication>
#include "document/BaseInsight.h"

namespace novelist {
    BaseInsight::BaseInsight(gsl::not_null<SceneDocument*> doc, int left, int right, QString msg)
            :m_cursor(doc),
             m_message(std::move(msg))
    {
        int const textLength = document()->toRawText().length();
        if (left < 0 || right < 0 || left > textLength || right > textLength)
            throw std::out_of_range("The range [" + std::to_string(left) + ", " + std::to_string(right) +
                    "] is not in the allowed range of the document [0, " + std::to_string(textLength) + "].");

        if (left > right)
            std::swap(left, right);

        m_cursor.setPosition(left);
        m_cursor.setPosition(right, QTextCursor::MoveMode::KeepAnchor);
    }

    SceneDocument* BaseInsight::document() const noexcept
    {
        return dynamic_cast<SceneDocument*>(m_cursor.document());
    }

    std::pair<int, int> BaseInsight::range() const noexcept
    {
        return std::make_pair(m_cursor.selectionStart(), m_cursor.selectionEnd());
    }

    QString const& BaseInsight::message() const noexcept
    {
        return m_message;
    }

    void BaseInsight::setMessage(QString const& msg) noexcept
    {
        m_message = msg;
    }

    void BaseInsight::postRemoveEvent() noexcept
    {
        QCoreApplication::postEvent(&document()->insightManager(), new internal::RemoveInsightEvent(this));
    }
}