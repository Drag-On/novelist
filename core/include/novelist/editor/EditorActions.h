/**********************************************************
 * @file   EditorActions.h
 * @author jan
 * @date   2/3/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_EDITORACTIONS_H
#define NOVELIST_EDITORACTIONS_H

#include <QAction>

namespace novelist::editor {
    struct EditorActions {
        QAction* m_undoAction = nullptr;
        QAction* m_redoAction = nullptr;
        QAction* m_copyAction = nullptr;
        QAction* m_cutAction = nullptr;
        QAction* m_pasteAction = nullptr;
        QAction* m_deleteAction = nullptr;
        QAction* m_selectAllAction = nullptr;
    };
}

#endif //NOVELIST_EDITORACTIONS_H
