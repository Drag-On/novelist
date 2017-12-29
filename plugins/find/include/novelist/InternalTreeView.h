/**********************************************************
 * @file   InternalTreeView.h
 * @author jan
 * @date   12/29/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_INTERNALTREEVIEW_H
#define NOVELIST_INTERNALTREEVIEW_H

#include <QtWidgets/QTreeView>

namespace novelist {
    namespace internal {
        class InternalTreeView : public QTreeView {
        public:
            using QTreeView::QTreeView;
            using QTreeView::moveCursor;
            using CursorAction = QTreeView::CursorAction;
        };
    }
}

#endif //NOVELIST_INTERNALTREEVIEW_H
