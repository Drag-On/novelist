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
        QAction* m_undoAction;
        QAction* m_redoAction;
    };
}

#endif //NOVELIST_EDITORACTIONS_H
