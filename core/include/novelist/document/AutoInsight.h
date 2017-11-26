/**********************************************************
 * @file   AutoInsight.h
 * @author jan
 * @date   10/30/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_AUTOINSIGHT_H
#define NOVELIST_AUTOINSIGHT_H

#include "BaseInsight.h"

namespace novelist {
    /**
     * Common base class for insights that are not user-generated, but rather by tools such as spellcheckers
     */
    class NOVELIST_CORE_EXPORT AutoInsight : public BaseInsight {
    Q_OBJECT

    public:
        /**
         * Construct insight on a document
         * @param doc Document
         * @param left Left position
         * @param right Right position
         * @param msg Message
         * @param suggestions A (possibly empty) list of suggestions to present to the user
         */
        AutoInsight(gsl::not_null<SceneDocument*> doc, int left, int right, QString msg,
                QStringList suggestions = QStringList());

        bool isPersistent() const noexcept override;

        QMenu const& menu() const noexcept override;

        void retranslate() noexcept override;

    private:
        QMenu m_menu;
        QStringList m_suggestions;
    };
}

#endif //NOVELIST_AUTOINSIGHT_H
