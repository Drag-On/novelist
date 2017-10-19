/**********************************************************
 * @file   IInspection.h
 * @author jan
 * @date   10/18/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_IINSPECTION_H
#define NOVELIST_IINSPECTION_H

#include <QtCore/QString>

namespace novelist {
    /**
     * Interface for all insights (notes, spell checking...)
     */
    class IInsight {
    public:
        virtual ~IInsight() = default;

        /**
         * Range of the insight in the document
         * @return Document position of first and last character included
         */
        virtual std::pair<int, int> range() const noexcept = 0;

        /**
         * @return Insight message
         */
        virtual QString const& message() const noexcept = 0;
    };
}

Q_DECLARE_METATYPE(novelist::IInsight*)

#endif //NOVELIST_IINSPECTION_H
