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
#include <QtWidgets/QMenu>
#include <QTextCursor>

namespace novelist {
    /**
     * Interface for all insights (notes, spell checking...)
     */
    class IInsight {
    public:
        virtual ~IInsight() = default;

        /**
         * @return Paragraphs spanned by this insight
         */
        virtual std::pair<int, int> parRange() const noexcept = 0;

        /**
         * Range of the insight in the document
         * @return Document position of first and last character included
         */
        virtual std::pair<int, int> range() const noexcept = 0;

        /**
         * @return A text cursor that spans the marked text
         */
        virtual QTextCursor toCursor() const noexcept = 0;

        /**
         * @return Insight message
         */
        virtual QString const& message() const noexcept = 0;

        /**
         * @return Human-readable type of this insight
         */
        virtual QString type() const noexcept = 0;

        /**
         * Provides a menu to show when the insight is right-clicked
         * @return The menu
         */
        virtual QMenu const& menu() const noexcept = 0;

    signals:
        /**
         * The insight's range has collapsed to zero length. It should probably be removed.
         */
        virtual void collapsed(IInsight*) = 0;
    };
}

Q_DECLARE_INTERFACE(novelist::IInsight, "novelist.IInsight")
Q_DECLARE_METATYPE(novelist::IInsight*)

#endif //NOVELIST_IINSPECTION_H
