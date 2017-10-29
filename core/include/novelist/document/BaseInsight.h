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
         * Construct insight
         * @param doc Document the insight is on
         * @param left Left position
         * @param right Right position
         * @param msg Message
         * @throw std::out_of_range if the provided range is not valid on the passed document
         */
        BaseInsight(gsl::not_null<SceneDocument*> doc, int left, int right, QString msg);

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

    protected:
        void setMessage(QString const& msg) noexcept;

        void postRemoveEvent() noexcept;

    private:
        QTextCursor m_cursor;
        QString m_message;
    };
}

#endif //NOVELIST_BASEINSIGHT_H
