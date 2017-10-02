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
#include <QTabBar>
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
         * Closes the tab of an open scene. Does nothing if the requested scene is not currently open.
         * Notifies the user if the scene has unsaved changes.
         * @param model Pointer to the model
         * @param index Index of a scene node of the model
         */
        void closeScene(ProjectModel* model, QModelIndex index);

        /**
         * Closes the tab with a particular index
         *
         * @param index Tab index
         */
        void closeScene(int index);

        /**
         * Closes all tabs.
         */
        void closeAll();

        /**
         * @param model Pointer to the model
         * @param index Index of a scene node of the model
         * @return Index of the page with the editor for the requested scene, or -1 if it cannot be found
         */
        int indexOf(ProjectModel const* model, QModelIndex index) const;

    signals:
        /**
         * Fires when the widget received focus or lost it
         * @param focused True when focus was gained, otherwise false
         */
        void focusReceived(bool focused);

    protected:
        void focusInEvent(QFocusEvent* event) override;

        void focusOutEvent(QFocusEvent* event) override;

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

        class InternalTabBar : public QTabBar {
            Q_OBJECT

        public:
            explicit InternalTabBar(SceneTabWidget* parent) noexcept
             : QTabBar(parent)
            {
            }

        protected:
            void focusInEvent(QFocusEvent* event) override
            {
                auto* tabWidget = dynamic_cast<SceneTabWidget*>(parent());
                emit tabWidget->focusReceived(true);

                QWidget::focusInEvent(event);
            }

            void focusOutEvent(QFocusEvent* event) override
            {
                auto* tabWidget = dynamic_cast<SceneTabWidget*>(parent());
                emit tabWidget->focusReceived(false);

                QWidget::focusOutEvent(event);
            }
        };
    }
}

#endif //NOVELIST_SCENETABWIDGET_H
