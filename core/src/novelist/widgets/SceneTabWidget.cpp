/**********************************************************
 * @file   SceneTabWidget.cpp
 * @author jan
 * @date   8/6/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <QTabBar>
#include "settings/Settings.h"
#include "widgets/SceneTabWidget.h"

namespace novelist {
    namespace internal {
        InternalTabBar::InternalTabBar(SceneTabWidget* parent) noexcept
                :QTabBar(parent)
        {
        }

        void InternalTabBar::focusInEvent(QFocusEvent* event)
        {
            auto* tabWidget = dynamic_cast<SceneTabWidget*>(parent());
            emit tabWidget->focusReceived(true);

            QWidget::focusInEvent(event);
        }

        void InternalTabBar::focusOutEvent(QFocusEvent* event)
        {
            auto* tabWidget = dynamic_cast<SceneTabWidget*>(parent());
            emit tabWidget->focusReceived(false);

            QWidget::focusOutEvent(event);
        }
    }

    SceneTabWidget::SceneTabWidget(QWidget* parent)
            :QTabWidget(parent)
    {
        setTabBar(new internal::InternalTabBar(this));
        setDocumentMode(true);
        setTabsClosable(true);
        setMovable(true);
        setElideMode(Qt::ElideRight);

        m_boldAction.setCheckable(true);
        m_italicAction.setCheckable(true);
        m_underlineAction.setCheckable(true);
        m_overlineAction.setCheckable(true);
        m_strikethroughAction.setCheckable(true);
        m_smallCapsAction.setCheckable(true);

        connect(this, &SceneTabWidget::tabCloseRequested, this, &SceneTabWidget::onTabCloseRequested);
        connect(this, &SceneTabWidget::currentChanged, this, &SceneTabWidget::onCurrentChanged);
        auto* editorPage = Settings::findPage("editor");
        if (editorPage)
            connect(editorPage, &SettingsPage::updateInitiated, this, &SceneTabWidget::onSettingsUpdate);
    }

    void SceneTabWidget::openScene(ProjectModel* model, QModelIndex index)
    {
        Expects(model->nodeType(index) == ProjectModel::NodeType::Scene);

        int tabIdx = indexOf(model, index);
        if (tabIdx == -1) {
            QSettings settings;
            SceneDocument* document = qvariant_cast<SceneDocument*>(model->data(index, ProjectModel::DocumentRole));
            bool prevModified = document->isModified();

            auto& editor = m_editors.emplace_back(new internal::InternalTextEditor(model->properties().m_lang));
            editor->setFocusPolicy(focusPolicy());
            editor->m_model = model;
            editor->m_modelIndex = index;
            editor->setDocument(document);
            document->setModified(prevModified); // setDocument() resets modified state
            editor->setWordWrapMode(QTextOption::WrapMode::WordWrap);
            editor->useInspectors(&m_inspectors);
            applySettingsToEditor(settings, editor.get());

            // Make sure the tab title and color change appropriately
            m_modelDataChangedConnections[editor->m_model] = connect(editor->m_model, &ProjectModel::dataChanged, this, &SceneTabWidget::onModelDataChanged);
            connect(editor.get(), &TextEditor::focusReceived, [this](bool focus) { emit focusReceived(focus); });

            QString name = model->data(index, Qt::DisplayRole).toString();
            tabIdx = addTab(editor.get(), name);
            tabBar()->setTabToolTip(tabIdx, name);
            tabBar()->setTabTextColor(tabIdx, qvariant_cast<QBrush>(model->data(index, Qt::ForegroundRole)).color());
        }
        setCurrentIndex(tabIdx);
    }

    void SceneTabWidget::closeScene(ProjectModel* model, QModelIndex index)
    {
        closeScene(indexOf(model, index));
    }

    void SceneTabWidget::closeScene(int index)
    {
        if (index < 0 || index > count())
            return;

        auto* w = dynamic_cast<internal::InternalTextEditor*>(widget(index));
        removeTab(index);

        if (w != nullptr) {
            m_editors.erase(std::remove_if(m_editors.begin(), m_editors.end(),
                    [this, w](std::unique_ptr<internal::InternalTextEditor> const& p) {
                        return p.get() == w;
                    }));
        }
    }

    void SceneTabWidget::closeAll()
    {
        for (int c = count(), i = 0; i < c; ++i)
            closeScene(0);
    }

    int SceneTabWidget::indexOf(ProjectModel const* model, QModelIndex index) const
    {
        for (int i = 0; i < count(); ++i) {
            auto* w = dynamic_cast<internal::InternalTextEditor*>(widget(i));
            if (w != nullptr && w->m_model == model && w->m_modelIndex == index)
                return i;
        }

        return -1;
    }

    void SceneTabWidget::useInsightView(QAbstractItemView* insightView)
    {
        m_insightView = insightView;
    }

    void SceneTabWidget::registerInspector(std::unique_ptr<Inspector> inspector)
    {
        m_inspectors.push_back(std::move(inspector));
    }

    QAction* SceneTabWidget::undoAction()
    {
        return &m_undoAction;
    }

    QAction* SceneTabWidget::redoAction()
    {
        return &m_redoAction;
    }

    QAction* SceneTabWidget::boldAction()
    {
        return &m_boldAction;
    }

    QAction* SceneTabWidget::italicAction()
    {
        return &m_italicAction;
    }

    QAction* SceneTabWidget::underlineAction()
    {
        return &m_underlineAction;
    }

    QAction* SceneTabWidget::overlineAction()
    {
        return &m_overlineAction;
    }

    QAction* SceneTabWidget::strikethroughAction()
    {
        return &m_strikethroughAction;
    }

    QAction* SceneTabWidget::smallCapsAction()
    {
        return &m_smallCapsAction;
    }

    QAction* SceneTabWidget::addNoteAction()
    {
        return &m_addNoteAction;
    }

    void SceneTabWidget::focusInEvent(QFocusEvent* event)
    {
        emit focusReceived(true);
        onCurrentChanged(currentIndex());

        QWidget::focusInEvent(event);
    }

    void SceneTabWidget::focusOutEvent(QFocusEvent* event)
    {
        emit focusReceived(false);

        QWidget::focusOutEvent(event);
    }

    void SceneTabWidget::onTabCloseRequested(int index)
    {
        closeScene(index);
    }

    void SceneTabWidget::onCurrentChanged(int /*index*/)
    {
        auto* editor = dynamic_cast<internal::InternalTextEditor*>(currentWidget());
        if (editor) {
            QString title = editor->m_model->data(editor->m_modelIndex, Qt::DisplayRole).toString();
            m_undoAction.setDelegate(editor, &TextEditor::undo, &TextEditor::canUndo, &TextEditor::undoAvailable,
                    tr("Undo modification of \"%1\"").arg(title));
            m_redoAction.setDelegate(editor, &TextEditor::redo, &TextEditor::canRedo, &TextEditor::redoAvailable,
                    tr("Redo modification of \"%1\"").arg(title));

            m_boldAction.setDelegate(editor->boldAction());
            m_italicAction.setDelegate(editor->italicAction());
            m_underlineAction.setDelegate(editor->underlineAction());
            m_overlineAction.setDelegate(editor->overlineAction());
            m_strikethroughAction.setDelegate(editor->strikethroughAction());
            m_smallCapsAction.setDelegate(editor->smallCapsAction());
            m_addNoteAction.setDelegate(editor->addNoteAction());

            if (m_insightView) {
                m_insightView->setModel(editor->insights());
            }
        }
        else {
            m_boldAction.setEnabled(false);
            m_italicAction.setEnabled(false);
            m_underlineAction.setEnabled(false);
            m_overlineAction.setEnabled(false);
            m_strikethroughAction.setEnabled(false);
            m_smallCapsAction.setEnabled(false);
            m_addNoteAction.setEnabled(false);

            if (m_insightView) {
                m_insightView->setModel(nullptr);
            }
        }
    }

    void SceneTabWidget::onModelDataChanged(QModelIndex const& topLeft, QModelIndex const& bottomRight)
    {
        auto const* model = dynamic_cast<ProjectModel const*>(topLeft.model());
        if (model == nullptr)
            return;

        for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {
            QModelIndex curIdx = model->index(i, 0, topLeft.parent());
            if (int tabIdx = indexOf(model, curIdx); tabIdx >= 0) {
                QString name = model->data(curIdx, Qt::DisplayRole).toString();
                tabBar()->setTabText(tabIdx, name);
                tabBar()->setTabToolTip(tabIdx, name);
                tabBar()->setTabTextColor(tabIdx,
                        qvariant_cast<QBrush>(model->data(curIdx, Qt::ForegroundRole)).color());
            }
        }
    }

    void SceneTabWidget::onSettingsUpdate()
    {
        QSettings settings;
        for (int i = 0; i < count(); ++i) {
            auto* w = dynamic_cast<TextEditor*>(widget(i));
            if (w != nullptr) {
                applySettingsToEditor(settings, w);
            }
        }
    }

    void SceneTabWidget::applySettingsToEditor(QSettings const& settings, TextEditor* editor) const
    {
        editor->setShowParagraphNumberArea(settings.value("editor/show_par_no", true).toBool());
        int widthLimit = settings.value("editor/width_limit").toInt();
        if (widthLimit > 0) {
            editor->setLineWrapMode(TextEditor::LineWrapMode::FixedPixelWidth);
            editor->setLineWrapColumnOrWidth(widthLimit);
        }
        else {
            editor->setLineWrapMode(TextEditor::LineWrapMode::WidgetWidth);
        }
    }
}