/**********************************************************
 * @file   MenuHelper.h
 * @author jan
 * @date   10/22/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_MENUHELPER_H
#define NOVELIST_MENUHELPER_H

#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>
#include <QtWidgets/QToolBar>

namespace novelist {
    /**
     * Replace an action in a menu with another one
     * @param menu Menu containing the action
     * @param old Action to remove
     * @param replacement Replacement action
     * @return Replacement action
     */
    QAction* replaceMenuAction(QMenu* menu, QAction* old, QAction* replacement);

    /**
     * Replace an action in a menu and a toolbar with another one
     * @param menu Menu containing the action
     * @param bar Toolbar containing the action
     * @param old Action to remove
     * @param replacement Replacement action
     * @return Replacement action
     */
    QAction* replaceMenuAndToolbarAction(QMenu* menu, QToolBar* bar, QAction* old, QAction* replacement);

    /**
     * Absorb a menu into another one, i.e. copy all actions and submenus into the top-level menu
     * @param dest Destination menu
     * @param src Source menu
     * @param before Action in \p dest before which \p src's actions are inserted. If null, add at end.
     */
    void absorbMenu(QMenu* dest, QMenu* src, QAction* before = nullptr);
}

#endif //NOVELIST_MENUHELPER_H
