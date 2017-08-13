/**********************************************************
 * @file   SceneTabWidget.h
 * @author jan
 * @date   8/6/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_SCENETABWIDGET_H
#define NOVELIST_SCENETABWIDGET_H

#include <QtWidgets/QTabWidget>
#include <QtCore/QFile>
#include "model/ProjectModel.h"
#include "TextEditor.h"

namespace novelist {
    class SceneTabWidget : public QTabWidget {
    Q_OBJECT

    public:
        explicit SceneTabWidget(QWidget* parent = nullptr);

        /**
         * If the scene is not open yet, loads it up from hard disk and creates a new tab for it. Makes the tab of the
         * scene active.
         * @param model Pointer to the model
         * @param index Index of a scene node of the model
         */
        void openScene(ProjectModel* model, QModelIndex index);

        /**
         * @param model Pointer to the model
         * @param index Index of a scene node of the model
         * @return Index of the page with the editor for the requested scene, or -1 if it cannot be found
         */
        int indexOf(ProjectModel const* model, QModelIndex index) const;

    private slots:

        void onTabCloseRequested(int index);
    };

    namespace internal {
        class InternalTextEditor : public TextEditor {
        Q_OBJECT

        public:
            using TextEditor::TextEditor;

            ProjectModel* m_model;
            QPersistentModelIndex m_modelIndex;
        };
    }
}

#endif //NOVELIST_SCENETABWIDGET_H
