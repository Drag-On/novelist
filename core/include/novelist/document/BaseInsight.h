/**********************************************************
 * @file   BaseInsight.h
 * @author jan
 * @date   10/28/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_BASEINSIGHT_H
#define NOVELIST_BASEINSIGHT_H

#include <QtGui/QTextCursor>
#include <gsl/gsl>
#include "Insight.h"
#include "InsightFactory.h"
#include "SceneDocument.h"

namespace novelist {

    /**
     * Implements some common functionality for insight specializations
     */
    class BaseInsight : public QObject, public Insight {
    Q_OBJECT
    Q_INTERFACES(novelist::Insight)

    public:
        /**
         * @return Underlying document
         */
        SceneDocument* document() const noexcept override;

        /**
         * @return Range covered on the document
         */
        std::pair<int, int> range() const noexcept override;

        /**
         * @return Message
         */
        QString const& message() const noexcept override;

        QString const& category() const noexcept override;

    protected:
        /**
         * Construct insight
         * @param doc Document the insight is on
         * @param left Left position
         * @param right Right position
         * @param msg Message
         * @throw std::out_of_range if the provided range is not valid on the passed document
         */
        BaseInsight(gsl::not_null<SceneDocument*> doc, int left, int right, QString msg);

        void setMessage(QString const& msg) noexcept;

        void setCategory(QString const& category) noexcept;

        void replaceMarkedText(QString const& text) noexcept;

        void postRemoveEvent() noexcept;

    private:
        QTextCursor m_cursor;
        QString m_message;
        QString m_category;

        template <typename, typename>
        friend class BaseInsightFactory;
    };
}

#endif //NOVELIST_BASEINSIGHT_H
