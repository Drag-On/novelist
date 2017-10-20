/**********************************************************
 * @file   ManualTextAnnotation.h
 * @author jan
 * @date   10/20/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_MANUALTEXTANNOTATION_H
#define NOVELIST_MANUALTEXTANNOTATION_H

#include "TextAnnotation.h"

namespace novelist {
    class ManualTextAnnotation : public TextAnnotation {
        Q_OBJECT

    public:
        /**
         * Construct a manual text annotation (a note)
         * @param doc Document
         * @param left Leftmost character
         * @param right Rightmost character
         * @param msg Message to show alongside the annotation
         */
        ManualTextAnnotation(gsl::not_null<QTextDocument*> doc, int left, int right, QString msg);

        /**
         * Provides a menu to show when the annotation is right-clicked
         * @return The menu
         */
        QMenu const& menu() const noexcept override;

    private:
        QMenu m_menu;
        QAction m_editAction;
        QAction m_removeAction;

        void onEditAction();

        void onRemoveAction();
    };
}

#endif //NOVELIST_MANUALTEXTANNOTATION_H
