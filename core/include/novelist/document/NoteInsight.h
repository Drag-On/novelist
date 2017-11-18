/**********************************************************
 * @file   NoteInsight.h
 * @author jan
 * @date   10/28/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_NOTEINSIGHT_H
#define NOVELIST_NOTEINSIGHT_H

#include "BaseInsight.h"

namespace novelist {
    /**
     * Implements a persistent note insight that is intended to be placed by the user manually
     */
    class NoteInsight : public BaseInsight {
        Q_OBJECT

    public:
        QTextCharFormat const& format() const noexcept override;

        bool isPersistent() const noexcept override;

        QMenu const& menu() const noexcept override;

        void retranslate() noexcept override;

    protected:
        /**
         * Construct insight on a document
         * @param doc Document
         * @param left Left end of range
         * @param right Right end of range
         * @param msg Message
         */
        NoteInsight(gsl::not_null<SceneDocument*> doc, int left, int right, QString msg);

    private:
        QMenu m_menu;
        QAction m_editAction;
        QAction m_removeAction;

        void onEditAction();

        void onRemoveAction();

        template <typename, typename>
        friend class BaseInsightFactory;
    };
}

#endif //NOVELIST_NOTEINSIGHT_H
