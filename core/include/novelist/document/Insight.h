/**********************************************************
 * @file   Insight.h
 * @author jan
 * @date   10/28/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_INSIGHT_H
#define NOVELIST_INSIGHT_H

#include <utility>
#include <QtGui/QTextCharFormat>
#include <QtWidgets/QMenu>

namespace novelist {
    /**
     * Interface class for all insights used with SceneDocument and TextEditor
     */
    class Insight {
    public:
        virtual ~Insight() noexcept = default;

        /**
         * @return Pointer to the document this insight is in
         */
        virtual QTextDocument* document() const noexcept = 0;

        /**
         * @return The marked range on the document
         */
        virtual std::pair<int, int> range() const noexcept = 0;

        /**
         * @return Character format used for highlighting
         */
        virtual QTextCharFormat const& format() const noexcept = 0;

        /**
         * @return Insight description
         */
        virtual QString const& message() const noexcept = 0;

        /**
         * @return Insight category
         */
        virtual QString const& category() const noexcept = 0;

        /**
         * @return True indicates that this insight should be stored to file, false indicates a temporary insight
         */
        virtual bool isPersistent() const noexcept = 0;

        /**
         * @return The menu to show when right-clicking the insight
         */
        virtual QMenu const& menu() const noexcept = 0;
    };

    /**
     * Compares Insights by their marked range. Left position is most relevant, then right position.
     */
    class InsightOrderCompare {
    public:
        bool operator() (Insight const& lhs, Insight const& rhs) {
            return lhs.range().first < rhs.range().first ||
                    (lhs.range().first == rhs.range().first && lhs.range().second < rhs.range().second);
        }
    };

    /**
     * @param insight An insight
     * @return Paragraphs spanned by this insight
     */
    std::pair<int, int> parRange(Insight const& insight) noexcept;

    /**
     * @param insight An insight
     * @return True in case the insight has zero-length, otherwise false
     */
    bool empty(Insight const& insight) noexcept;

    /**
     * @param insight An insight
     * @return Amount of characters spanned by the insight
     */
    int length(Insight const& insight) noexcept;
}

Q_DECLARE_INTERFACE(novelist::Insight, "novelist::Insight")
Q_DECLARE_METATYPE(novelist::Insight*)

#endif //NOVELIST_INSIGHT_H
