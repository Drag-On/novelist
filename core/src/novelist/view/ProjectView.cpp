/**********************************************************
 * @file   ProjectModelView.cpp
 * @author jan
 * @date   7/21/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <QPainter>
#include "view/ProjectView.h"

// The macro cannot be called from within a namespace (see http://doc.qt.io/qt-5/qdir.html#Q_INIT_RESOURCE)
inline void initResources() { Q_INIT_RESOURCE(resources); }

QIcon overlayThemeIcon(QString baseResPath, QString iconThemeName, float factor = 0.5f)
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
        m_newSceneButton->setToolTip(tr("Create a new scene"));
        m_newChapterButton->setToolTip(tr("Create a new chapter"));
        m_deleteButton->setToolTip(tr("Remove a scene or chapter"));
    }

    void ProjectView::setup()
    {
        initResources();

        m_topLayout = new QVBoxLayout(this);
        m_treeView = new QTreeView(this);
        m_treeView->setEditTriggers(QAbstractItemView::EditKeyPressed);
        m_treeView->setDragEnabled(true);
        m_treeView->setDragDropMode(QAbstractItemView::InternalMove);
        m_treeView->setDefaultDropAction(Qt::TargetMoveAction);
        m_treeView->setIconSize(QSize(16, 16));
        m_treeView->setRootIsDecorated(true);
        m_treeView->setUniformRowHeights(false);
        m_treeView->setAnimated(true);
        m_treeView->setHeaderHidden(true);

        m_topLayout->addWidget(m_treeView);

        m_nestedLayout = new QHBoxLayout();
        m_newSceneButton = new QToolButton(this);
        m_newSceneButton->setEnabled(false);
        m_newSceneButton->setIcon(overlayThemeIcon(":/icons/scene.png", "list-add", 0.7f));

        m_nestedLayout->addWidget(m_newSceneButton);

        m_newChapterButton = new QToolButton(this);
        m_newChapterButton->setEnabled(false);
        m_newChapterButton->setIcon(overlayThemeIcon(":/icons/chapter.png", "list-add", 0.7f));

        m_nestedLayout->addWidget(m_newChapterButton);

        m_deleteButton = new QToolButton(this);
        m_deleteButton->setEnabled(false);
        QIcon iconRemove;
        QString iconThemeName = QStringLiteral("list-remove");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            iconRemove = QIcon::fromTheme(iconThemeName);
        }
        else {
            iconRemove.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
        }
        m_deleteButton->setIcon(iconRemove);

        m_nestedLayout->addWidget(m_deleteButton);

        m_buttonSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        m_nestedLayout->addItem(m_buttonSpacer);

        m_topLayout->addLayout(m_nestedLayout);

        retranslateUi();
    }
}