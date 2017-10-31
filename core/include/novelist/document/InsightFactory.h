/**********************************************************
 * @file   InsightFactory.h
 * @author jan
 * @date   10/30/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_INSIGHTFACTORY_H
#define NOVELIST_INSIGHTFACTORY_H

#include <memory>
#include <type_traits>
#include <gsl/gsl>
#include "Insight.h"
#include "SceneDocument.h"
#include "AutoInsight.h"

namespace novelist {
    /**
     * Produces an insight
     */
    class InsightFactory {
    public:
        virtual ~InsightFactory() noexcept = default;

        /**
         * Create an insight on the specified document
         * @param doc Document to create insight for
         * @param left Left position
         * @param right Right position
         * @return The insight or a nullptr if it could not be created
         */
        virtual std::unique_ptr<Insight> create(gsl::not_null<SceneDocument*> doc, int left, int right) noexcept = 0;
    };

    /**
     * A general insight factory for all insight types that are fully described by a message.
     * @tparam T Insight type
     */
    template<typename T, typename = std::enable_if<std::is_convertible_v<T*, BaseInsight*>>>
    class BaseInsightFactory : public InsightFactory {
    public:
        explicit BaseInsightFactory(QString msg)
                :m_msg(std::move(msg)) { }

        std::unique_ptr<Insight> create(gsl::not_null<SceneDocument*> doc, int left, int right) noexcept override
        {
            std::unique_ptr<Insight> obj;
            try {
                obj = std::make_unique<T>(doc, left, right, m_msg);
            }
            catch (...) {
            }
            return obj;
        }

    protected:
        QString const& message() const { return m_msg; }

    private:
        QString m_msg;
    };

    /**
     * A general insight factory for all insight types that are fully described by a message and a list of suggestions.
     * @tparam T Insight type
     */
    template<typename T, typename = std::enable_if<std::is_convertible_v<T*, AutoInsight*>>>
    class AutoInsightFactory : public BaseInsightFactory<T> {
    public:
        explicit AutoInsightFactory(QString msg, QStringList suggestions)
                :BaseInsightFactory<T>(std::move(msg)),
                m_suggestions(std::move(suggestions)) { }

        std::unique_ptr<Insight> create(gsl::not_null<SceneDocument*> doc, int left, int right) noexcept override
        {
            std::unique_ptr<Insight> obj;
            try {
                obj = std::make_unique<T>(doc, left, right, BaseInsightFactory<T>::message(), m_suggestions);
            }
            catch (...) {
            }
            return obj;
        }

    private:
        QStringList m_suggestions;
    };
}

#endif //NOVELIST_INSIGHTFACTORY_H
