/**********************************************************
 * @file   MenuHelper.cpp
 * @author jan
 * @date   10/22/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "util/MenuHelper.h"

namespace novelist {
    QAction* replaceMenuAction(QMenu* menu, QAction* old, QAction* replacement)
    {
        replacement->setIcon(old->icon());

        menu->insertAction(old, replacement);
        menu->removeAction(old);

        return replacement;
    }

    QAction* replaceMenuAndToolbarAction(QMenu* menu, QToolBar* bar, QAction* old, QAction* replacement)
    {
        replacement->setIcon(old->icon());

        menu->insertAction(old, replacement);
        bar->insertAction(old, replacement);
        menu->removeAction(old);
        bar->removeAction(old);

        return replacement;
    }

    void absorbMenu(QMenu* dest, QMenu* src, QAction* before)
    {
        for (QAction* a : src->actions()) {
            if (before)
                dest->insertAction(before, a);
            else
                dest->addAction(a);
        }
    }
}