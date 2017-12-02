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
#include <QtWidgets/QAbstractItemView>
#include <QSettings>
#include <vector>
#include "util/DelegateAction.h"
#include "util/ConnectionWrapper.h"
#include "model/ProjectModel.h"
#include "widgets/texteditor/Inspector.h"
#include "widgets/texteditor/TextEditor.h"
#include <novelist_core_export.h>

namespace novelist {
    class SceneTabWidget;

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
            explicit InternalTabBar(SceneTabWidget* parent) noexcept;

        protected:
            void focusInEvent(QFocusEvent* event) override;

            void focusOutEvent(QFocusEvent* event) override;
        };
    }

    class NOVELIST_CORE_EXPORT SceneTabWidget : public QTabWidget {
    Q_OBJECT

    public:
        /**
         * Construct the scene tab widget
         * @param parent Parent widget, may be nullptr.
         */
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

        /**
         * @param insightView Pointer to the view that is supposed to show the insights of the currently open document.
         *                    May be nullptr, but if it isn't the pointer must stay valid during the lifetime of this
         *                    tab widget.
         */
        void useInsightView(QAbstractItemView* insightView);

        /**
         * Registers an inspector that will run on all scene text editors
         * @param inspector New inspector
         */
        void registerInspector(std::unique_ptr<Inspector> inspector);

        /**
         * Provides an action that undoes changes in the currently open document, if any
         * @note Ownership remains with the tab widget
         * @return Requested action
         */
        QAction* undoAction();

        /**
         * Provides an action that redoes changes in the currently open document, if any
         * @note Ownership remains with the tab widget
         * @return Requested action
         */
        QAction* redoAction();

        /**
         * Provides an action that makes the currently selected text bold (or reverts that)
         * @return Non-owning pointer to the action
         */
        QAction* boldAction();

        /**
         * Provides an action that makes the currently selected text italic (or reverts that)
         * @return Non-owning pointer to the action
         */
        QAction* italicAction();

        /**
         * Provides an action that makes the currently selected text underlined (or reverts that)
         * @return Non-owning pointer to the action
         */
        QAction* underlineAction();

        /**
         * Provides an action that makes the currently selected text overlined (or reverts that)
         * @return Non-owning pointer to the action
         */
        QAction* overlineAction();

        /**
         * Provides an action that makes the currently selected text strike through (or reverts that)
         * @return Non-owning pointer to the action
         */
        QAction* strikethroughAction();

        /**
         * Provides an action that makes the currently selected text small caps (or reverts that)
         * @return Non-owning pointer to the action
         */
        QAction* smallCapsAction();

        /**
         * Provides an action that makes the currently selected text into a note
         * @return  Non-owning pointer to the action
         */
        QAction* addNoteAction();

    signals:
        /**
         * Fires when the widget received focus or lost it
         * @param focused True when focus was gained, otherwise false
         */
        void focusReceived(bool focused);

        /**
         * Fires whenever a new scene editor is opened
         * @param editor Pointer to the new editor
         */
        void sceneOpened(gsl::not_null<TextEditor*> editor);

        /**
         * Fires whenever a scene editor is closed
         * @param editor Pointer to the closing editor
         */
        void sceneClosing(gsl::not_null<TextEditor*> editor);

    protected:
        void focusInEvent(QFocusEvent* event) override;

        void focusOutEvent(QFocusEvent* event) override;

    private slots:

        void onTabCloseRequested(int index);

        void onCurrentChanged(int index);

        void onModelDataChanged(QModelIndex const& topLeft, QModelIndex const& bottomRight);

        void onSettingsUpdate();

    private:
        DelegateAction m_undoAction;
        DelegateAction m_redoAction;
        DelegateAction m_boldAction;
        DelegateAction m_italicAction;
        DelegateAction m_underlineAction;
        DelegateAction m_overlineAction;
        DelegateAction m_strikethroughAction;
        DelegateAction m_smallCapsAction;
        DelegateAction m_addNoteAction;
        QAbstractItemView* m_insightView = nullptr;
        std::vector<std::unique_ptr<internal::InternalTextEditor>> m_editors;
        std::vector<std::unique_ptr<Inspector>> m_inspectors;
        std::map<ProjectModel*, ConnectionWrapper> m_modelDataChangedConnections;

        void applySettingsToEditor(QSettings const& settings, TextEditor* editor) const;
    };
}

#endif //NOVELIST_SCENETABWIDGET_H
