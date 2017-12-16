/**********************************************************
 * @file   ProjectModelView.cpp
 * @author jan
 * @date   7/21/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <QPainter>
#include <QDrag>
#include <QMessageBox>
#include "windows/ProjectPropertiesWindow.h"
#include "view/ProjectView.h"

// The macro cannot be called from within a namespace (see http://doc.qt.io/qt-5/qdir.html#Q_INIT_RESOURCE)
inline void initResources() { Q_INIT_RESOURCE(resources); }

QIcon overlayThemeIcon(QString const& baseResPath, QString const& iconThemeName, float factor = 0.5f)
{
    QPixmap base(baseResPath);
    QIcon addIcon;
    if (QIcon::hasThemeIcon(iconThemeName)) {
        addIcon = QIcon::fromTheme(iconThemeName);
    }
    else {
        return base;
    }
    QPixmap overlay = addIcon.pixmap(addIcon.actualSize(base.size()));
    QPixmap result(base.width(), base.height());
    result.fill(Qt::transparent);
    {
        auto const x = base.width() - static_cast<int const>(base.width() * factor);
        auto const y = base.height() - static_cast<int const>(base.height() * factor);
        auto const w = static_cast<int const>(base.width() * factor);
        auto const h = static_cast<int const>(base.height() * factor);
        QPainter painter(&result);
        painter.drawPixmap(0, 0, base);
        painter.drawPixmap(x, y, w, h, overlay);
    }
    return result;
}

namespace novelist {
    ProjectView::ProjectView(QWidget* parent)
            :QWidget(parent)
    {
        setup();
    }

    void ProjectView::retranslateUi()
    {
        m_newSceneButton->setToolTip(tr("Create scene"));
        m_newChapterButton->setToolTip(tr("Create chapter"));
        m_deleteButton->setToolTip(tr("Remove scene or chapter"));
        m_propertiesButton->setToolTip(tr("Properties"));

        m_actionNewChapter->setText(tr("Create &Chapter"));
        // m_actionNewChapter->setShortcut(QApplication::translate("ProjectView", "Ctrl+Shift+N", Q_NULLPTR));
        m_actionNewScene->setText(tr("Create &Scene"));
        // m_actionNewScene->setShortcut(QApplication::translate("ProjectView", "Ctrl+N", Q_NULLPTR));
        m_actionRemoveEntry->setText(tr("&Remove"));
        m_actionProperties->setText(tr("&Properties"));
    }

    void ProjectView::setModel(ProjectModel* model)
    {
        emit modelAboutToChange(this->model());
        m_treeView->setModel(model);
        connect(m_treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this,
                &ProjectView::onSelectionChanged);
        if(model != nullptr) {
            m_actionUndo = model->undoStack().createUndoAction(this);
            m_actionRedo = model->undoStack().createRedoAction(this);
        }
        emit modelChanged(model);
    }

    ProjectModel* ProjectView::model() const
    {
        return dynamic_cast<ProjectModel*>(m_treeView->model());
    }

    QDialog::DialogCode ProjectView::showProjectPropertiesDialog()
    {
        auto* m = model();
        if (m != nullptr) {
            ProjectPropertiesWindow wnd(this, Qt::WindowFlags());
            wnd.setProperties(m->properties());
            QDialog::DialogCode code = static_cast<QDialog::DialogCode>(wnd.exec());
            if (code == QDialog::Accepted)
                m->setProperties(wnd.properties());
            return code;
        }
        return QDialog::DialogCode::Rejected;
    }

    QAction* ProjectView::undoAction() const
    {
        return m_actionUndo;
    }

    QAction* ProjectView::redoAction() const
    {
        return m_actionRedo;
    }

    void ProjectView::onNewChapter()
    {
        ProjectModel* m = model();
        Q_ASSERT(m);

        if (m_treeView->selectionModel()->selectedIndexes().empty())
            return;

        auto idx = m_treeView->selectionModel()->selectedIndexes().first();
        Q_ASSERT(idx.isValid());

        static size_t num = 1;

        switch (model()->nodeType(idx)) {
            case ProjectModel::NodeType::Chapter:
            case ProjectModel::NodeType::Scene: {
                int const row = idx.row() + 1;
                auto const parent = idx.parent();
                m->insertRow(row, ProjectModel::InsertableNodeType::Chapter, tr("Chapter %1").arg(num++), parent);
                break;
            }
            default: {
                int const row = model()->rowCount(idx);
                m->insertRow(row, ProjectModel::InsertableNodeType::Chapter, tr("Chapter %1").arg(num++), idx);
                m_treeView->setExpanded(idx, true);
                break;
            }
        }
    }

    void ProjectView::onNewScene()
    {
        ProjectModel* m = model();
        Q_ASSERT(m);

        if (m_treeView->selectionModel()->selectedIndexes().empty())
            return;

        auto idx = m_treeView->selectionModel()->selectedIndexes().first();
        Q_ASSERT(idx.isValid());

        static size_t num = 1;

        switch (model()->nodeType(idx)) {
            case ProjectModel::NodeType::Scene: {
                int const row = idx.row() + 1;
                auto const parent = idx.parent();
                m->insertRow(row, ProjectModel::InsertableNodeType::Scene, tr("Scene %1").arg(num++), parent);
                break;
            }
            default: {
                int const row = model()->rowCount(idx);
                m->insertRow(row, ProjectModel::InsertableNodeType::Scene, tr("Scene %1").arg(num++), idx);
                m_treeView->setExpanded(idx, true);
                break;
            }
        }
    }

    void ProjectView::onRemoveEntry()
    {
        ProjectModel* m = model();
        Q_ASSERT(m);

        if (m_treeView->selectionModel()->selectedIndexes().empty())
            return;

        auto idx = m_treeView->selectionModel()->selectedIndexes().first();
        Q_ASSERT(idx.isValid());

        auto parIdx = idx.parent();
        auto row = idx.row();
        m->removeRow(row, parIdx);

        // Select another item
        if (m->rowCount(parIdx) == 0) // Select parent if no more children
            m_treeView->selectionModel()->select(parIdx, QItemSelectionModel::SelectCurrent);
        else if (row >= m->rowCount(parIdx)) // Select item before if no more items after
            m_treeView->selectionModel()->select(parIdx.child(m->rowCount(parIdx) - 1, 0),
                    QItemSelectionModel::SelectCurrent);
        else // Select item after
            m_treeView->selectionModel()->select(parIdx.child(row, 0), QItemSelectionModel::SelectCurrent);
    }

    void ProjectView::onProperties()
    {
        ProjectModel* m = model();
        Q_ASSERT(m);

        if (m_treeView->selectionModel()->selectedIndexes().empty())
            return;

        auto idx = m_treeView->selectionModel()->selectedIndexes().first();
        Q_ASSERT(idx.isValid());

        auto nodeType = m->nodeType(idx);
        switch (nodeType)
        {
            case ProjectModel::NodeType::ProjectHead:
            {
                showProjectPropertiesDialog();
            }
            default:
                break;
        }
    }

    void ProjectView::onDoubleClick(QModelIndex idx)
    {
        Expects(idx.isValid());

        ProjectModel* m = model();
        Q_ASSERT(m);

        auto nodeType = m->nodeType(idx);
        switch (nodeType)
        {
            case ProjectModel::NodeType::Scene:
            {
                emit openSceneRequested(idx);
                break;
            }
            default:
                break;
        }
    }

    void ProjectView::onSelectionChanged(QItemSelection const& selected, QItemSelection const& /*deselected*/)
    {
        if (m_treeView->selectionModel()->selectedIndexes().empty()) {
            m_contextMenu->setDisabled(true);
            m_actionNewChapter->setEnabled(false);
            m_actionNewScene->setEnabled(false);
            m_actionRemoveEntry->setEnabled(false);
            m_actionProperties->setEnabled(false);
        }
        else {
            ProjectModel* m = model();
            m_contextMenu->setEnabled(true);

            Q_ASSERT(selected.size() <= 1);

            QModelIndex idx = m_treeView->selectionModel()->selectedIndexes().first();
            switch (m->nodeType(idx)) {
                case ProjectModel::NodeType::ProjectHead:
                    // Is project root
                    m_actionNewChapter->setEnabled(true);
                    m_actionNewScene->setEnabled(true);
                    m_actionRemoveEntry->setEnabled(false);
                    m_actionProperties->setEnabled(true);
                    break;
                case ProjectModel::NodeType::NotebookHead:
                    // Is notebook root
                    m_actionNewChapter->setEnabled(true);
                    m_actionNewScene->setEnabled(true);
                    m_actionRemoveEntry->setEnabled(false);
                    m_actionProperties->setEnabled(false);
                    break;
                case ProjectModel::NodeType::Chapter:
                    // Is a chapter
                    m_actionNewChapter->setEnabled(true);
                    m_actionNewScene->setEnabled(true);
                    m_actionRemoveEntry->setEnabled(true);
                    m_actionProperties->setEnabled(false);
                    break;
                case ProjectModel::NodeType::Scene:
                    // Is a document
                    m_actionNewChapter->setEnabled(true);
                    m_actionNewScene->setEnabled(true);
                    m_actionRemoveEntry->setEnabled(true);
                    m_actionProperties->setEnabled(false);
                    break;
                default:
                    m_actionNewChapter->setEnabled(false);
                    m_actionNewScene->setEnabled(false);
                    m_actionRemoveEntry->setEnabled(false);
                    m_actionProperties->setEnabled(false);
                    break;
            }

        }
    }

    void ProjectView::onContextMenuEvent(QPoint const& pos)
    {
        QModelIndex index = m_treeView->indexAt(pos);
        if (index.isValid())
            m_contextMenu->popup(m_treeView->viewport()->mapToGlobal(pos));
    }

    void ProjectView::focusInEvent(QFocusEvent* event)
    {
        emit focusReceived(true);

        QWidget::focusInEvent(event);
    }

    void ProjectView::focusOutEvent(QFocusEvent* event)
    {
        emit focusReceived(false);

        QWidget::focusOutEvent(event);
    }

    void ProjectView::setup()
    {
        initResources();

        m_iconNewScene = overlayThemeIcon(":/icons/node-scene", "list-add", 0.7f);
        m_iconNewChapter = overlayThemeIcon(":/icons/node-chapter", "list-add", 0.7f);
        QString iconThemeName = QStringLiteral("list-remove");
        if (QIcon::hasThemeIcon(iconThemeName))
            m_iconRemove = QIcon::fromTheme(iconThemeName);
        else
            m_iconRemove.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
        QString const iconConfigTheme = QStringLiteral("document-properties");
        if (QIcon::hasThemeIcon(iconConfigTheme))
            m_iconConfig = QIcon::fromTheme(iconConfigTheme);
        else
            m_iconConfig.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);

        setupActions();

        m_topLayout = new QVBoxLayout(this);
        m_treeView = new internal::ProjectTreeView(this);
        m_treeView->setEditTriggers(QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);
        m_treeView->setSelectionMode(QAbstractItemView::SingleSelection);
        m_treeView->setDragEnabled(true);
        m_treeView->viewport()->setAcceptDrops(true);
        m_treeView->setDragDropOverwriteMode(false);
        m_treeView->setDragDropMode(QAbstractItemView::InternalMove);
        m_treeView->setDefaultDropAction(Qt::MoveAction);
        m_treeView->setDropIndicatorShown(true);
        m_treeView->setIconSize(QSize(16, 16));
        m_treeView->setRootIsDecorated(true);
        m_treeView->setUniformRowHeights(false);
        m_treeView->setAnimated(true);
        m_treeView->setHeaderHidden(true);
        m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);

        m_topLayout->addWidget(m_treeView);

        m_nestedLayout = new QHBoxLayout();

        // Note: Adding actions to the buttons directly always makes them show text. I have no idea how to turn this off.
        m_newSceneButton = new QToolButton(this);
        m_newSceneButton->setEnabled(false);
        m_newSceneButton->setIcon(m_iconNewScene);
        m_nestedLayout->addWidget(m_newSceneButton);

        m_newChapterButton = new QToolButton(this);
        m_newChapterButton->setEnabled(false);
        m_newChapterButton->setIcon(m_iconNewChapter);
        m_nestedLayout->addWidget(m_newChapterButton);

        m_deleteButton = new QToolButton(this);
        m_deleteButton->setEnabled(false);
        m_deleteButton->setIcon(m_iconRemove);
        m_nestedLayout->addWidget(m_deleteButton);

        m_buttonSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        m_nestedLayout->addItem(m_buttonSpacer);

        m_propertiesButton = new QToolButton(this);
        m_propertiesButton->setEnabled(false);
        m_propertiesButton->setIcon(m_iconConfig);
        m_nestedLayout->addWidget(m_propertiesButton);

        m_topLayout->addLayout(m_nestedLayout);

        m_contextMenu = new QMenu(this);
        m_contextMenu->addAction(m_actionNewChapter);
        m_contextMenu->addAction(m_actionNewScene);
        m_contextMenu->addAction(m_actionRemoveEntry);
        m_contextMenu->addSeparator();
        m_contextMenu->addAction(m_actionProperties);

        setupConnections();

        retranslateUi();
    }

    void ProjectView::setupActions()
    {
        m_actionNewChapter = new QAction(this);
        m_actionNewChapter->setIcon(m_iconNewChapter);
        m_actionNewChapter->setIconVisibleInMenu(true);
        m_actionNewChapter->setEnabled(false);

        m_actionNewScene = new QAction(this);
        m_actionNewScene->setIcon(m_iconNewScene);
        m_actionNewScene->setIconVisibleInMenu(true);
        m_actionNewScene->setEnabled(false);

        m_actionRemoveEntry = new QAction(this);
        m_actionRemoveEntry->setIcon(m_iconRemove);
        m_actionRemoveEntry->setIconVisibleInMenu(true);
        m_actionRemoveEntry->setEnabled(false);

        m_actionProperties = new QAction(this);
        m_actionProperties->setIcon(m_iconConfig);
        m_actionProperties->setIconVisibleInMenu(true);
        m_actionProperties->setEnabled(false);
    }

    void ProjectView::setupConnections()
    {
        connect(m_treeView, &QTreeView::customContextMenuRequested, this, &ProjectView::onContextMenuEvent);
        connect(m_treeView, &QTreeView::doubleClicked, this, &ProjectView::onDoubleClick);

        connect(m_newChapterButton, &QToolButton::clicked, m_actionNewChapter, &QAction::trigger);
        connect(m_newSceneButton, &QToolButton::clicked, m_actionNewScene, &QAction::trigger);
        connect(m_deleteButton, &QToolButton::clicked, m_actionRemoveEntry, &QAction::trigger);
        connect(m_propertiesButton, &QToolButton::clicked, m_actionProperties, &QAction::trigger);

        connect(m_actionNewChapter, &QAction::triggered, this, &ProjectView::onNewChapter);
        connect(m_actionNewScene, &QAction::triggered, this, &ProjectView::onNewScene);
        connect(m_actionRemoveEntry, &QAction::triggered, this, &ProjectView::onRemoveEntry);
        connect(m_actionProperties, &QAction::triggered, this, &ProjectView::onProperties);

        connect(m_actionNewChapter, &QAction::changed,
                [&]() { m_newChapterButton->setEnabled(m_actionNewChapter->isEnabled()); });
        connect(m_actionNewScene, &QAction::changed,
                [&]() { m_newSceneButton->setEnabled(m_actionNewScene->isEnabled()); });
        connect(m_actionRemoveEntry, &QAction::changed,
                [&]() { m_deleteButton->setEnabled(m_actionRemoveEntry->isEnabled()); });
        connect(m_actionProperties, &QAction::changed,
                [&]() { m_propertiesButton->setEnabled(m_actionProperties->isEnabled()); });

        connect(this, &ProjectView::focusReceived, [this] (bool focus) { setProperty("focused", focus); update(); });
    }

    void ProjectView::paintEvent(QPaintEvent* event)
    {
        QWidget::paintEvent(event);

        if(property("focused").toBool()) {
            QPainter painter(this);
            painter.setPen(QPen(QBrush(m_focusColor), 4, Qt::SolidLine));
            painter.drawRect(m_treeView->rect().translated(m_treeView->pos()));
        }
    }

    void ProjectView::changeEvent(QEvent* event)
    {
        QWidget::changeEvent(event);
        switch (event->type()) {
            case QEvent::LanguageChange:
                retranslateUi();
                break;
            default:
                break;
        }
    }

    internal::ProjectTreeView::ProjectTreeView(ProjectView* parent) noexcept
        : QTreeView(parent)
    {
    }

    void internal::ProjectTreeView::startDrag(Qt::DropActions supportedActions)
    {
        QModelIndexList indexes = selectionModel()->selectedRows();
        if (indexes.count() > 0) {
            QMimeData* data = model()->mimeData(indexes);
            if (!data)
                return;
            QRect rect = visualRect(indexes.first());
            QPixmap pixmap(rect.size());
            render(&pixmap, QPoint(), rect);

            auto* drag = new QDrag(this);
            drag->setPixmap(pixmap);
            drag->setMimeData(data);

            drag->exec(supportedActions, this->defaultDropAction());
        }
    }

    void internal::ProjectTreeView::focusInEvent(QFocusEvent* event)
    {
        auto* view = dynamic_cast<ProjectView*>(parent()); // Safe because constructor only takes ProjectView*
        emit view->focusReceived(true);

        QAbstractItemView::focusInEvent(event);
    }

    void internal::ProjectTreeView::focusOutEvent(QFocusEvent* event)
    {
        auto* view = dynamic_cast<ProjectView*>(parent()); // Safe because constructor only takes ProjectView*
        emit view->focusReceived(false);

        QAbstractItemView::focusOutEvent(event);
    }
}