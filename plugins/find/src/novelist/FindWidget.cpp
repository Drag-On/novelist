/**********************************************************
 * @file   FindWidget.cpp
 * @author jan
 * @date   12/17/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtCore/QEvent>
#include <QtWidgets/QProgressDialog>
#include <QtCore/QTime>
#include <QtGui/QStandardItemModel>
#include <windows/MainWindow.h>
#include <util/Overloaded.h>
#include <QtGui/QtGui>
#include "FindWidget.h"
#include "ui_FindWidget.h"

namespace novelist {
    FindWidget::FindWidget(QWidget* parent) noexcept
            :QWidget(parent),
             m_ui(std::make_unique<Ui::FindWidget>())
    {
        m_ui->setupUi(this);
        setupConnections();
    }

    void FindWidget::changeEvent(QEvent* event)
    {
        QWidget::changeEvent(event);
        switch (event->type()) {
            case QEvent::LanguageChange:
                m_ui->retranslateUi(this);
                break;
            default:
                break;
        }
    }

    void FindWidget::setupConnections() noexcept
    {
        connect(m_ui->pushButtonSearch, &QPushButton::pressed, this, &FindWidget::onSearchStarted);
    }

    std::pair<ProjectModel*, QModelIndex> FindWidget::getSearchModelRoot() noexcept
    {
        std::pair<ProjectModel*, QModelIndex> result;

        MainWindow* mainWin = nullptr;
        for (QWidget* w : qApp->topLevelWidgets()) {
            auto* window = dynamic_cast<MainWindow*>(w);
            if (window) {
                mainWin = window;
                break;
            }
        }

        if (mainWin == nullptr)
            return result;

        result.first = mainWin->project();

        switch (m_ui->comboBoxScope->currentIndex()) {
            case 0: // Scene
            {
                auto[m, idx] = mainWin->sceneTabWidget()->current();
                if (m == result.first) {
                    result.second = idx;
                    return result;
                }
                break;
            }
            case 1: // Chapter
            {
                auto[m, idx] = mainWin->sceneTabWidget()->current();
                if (m == result.first) {
                    result.second = idx.parent();
                    return result;
                }
                break;
            }
            case 2: // Project
            {
                result.second = result.first->projectRootIndex();
                break;
            }
            default:
                break;
        }

        return result;
    }

    void FindWidget::search(ProjectModel* model, QModelIndex root, QStandardItemModel& resultsModel,
            QStandardItem* resultModelRoot, QProgressDialog& dialog) noexcept
    {
        Expects(model != nullptr);
        Expects(root.isValid());

        if (dialog.wasCanceled())
            return;

        bool matchCase = m_ui->checkBoxMatchCase->isChecked();
        bool regex = m_ui->checkBoxRegEx->isChecked();
        bool searchTitles = m_ui->checkBoxSearchTitles->isChecked();
        QString searchPhrase = m_ui->lineEditFind->text();

        using ProjectHeadData = ProjectModel::ProjectHeadData;
        using SceneData = ProjectModel::SceneData;
        using ChapterData = ProjectModel::ChapterData;

        std::visit(Overloaded {
                [](auto&) { qWarning() << "Can't search invalid node type."; },
                [&](ProjectHeadData& arg) {
                    QStandardItem* item = new QStandardItem(QIcon(":/icons/node-project"), arg.m_properties.m_name.toHtmlEscaped());
                    resultModelRoot->appendRow(item);
                    int const childCount = model->rowCount(root);
                    dialog.setMaximum(dialog.maximum() + childCount);
                    dialog.setValue(dialog.value() + 1);
                    for (int i = 0; i < childCount; ++i)
                        search(model, root.child(i, 0), resultsModel, item, dialog);
                },
                [&](ChapterData& arg) {
                    QStandardItem* item = new QStandardItem(QIcon(":/icons/node-chapter"), arg.m_name.toHtmlEscaped());
                    resultModelRoot->appendRow(item);
                    int const childCount = model->rowCount(root);
                    dialog.setMaximum(dialog.maximum() + childCount);
                    if (searchTitles) {
                        QStandardItem* titleItem = new QStandardItem("<i>" + tr("Title") + "</i>");
                        item->appendRow(titleItem);
                        auto titleResults = find(arg.m_name, searchPhrase, matchCase, regex);
                        addTitleResults(root, resultsModel, titleItem, titleResults, arg.m_name);
                    }
                    dialog.setValue(dialog.value() + 1);
                    for (int i = 0; i < childCount; ++i)
                        search(model, root.child(i, 0), resultsModel, item, dialog);
                },
                [&](SceneData& arg) {
                    // TODO: This is a placeholder
                    QTime dieTime = QTime::currentTime().addMSecs(300);
                    while (QTime::currentTime() < dieTime)
                        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

                    QStandardItem* item = new QStandardItem(QIcon(":/icons/node-scene"), arg.m_name.toHtmlEscaped());
                    resultModelRoot->appendRow(item);
                    if (searchTitles) {
                        QStandardItem* titleItem = new QStandardItem("<i>" + tr("Title") + "</i>");
                        item->appendRow(titleItem);
                        auto titleResults = find(arg.m_name, searchPhrase, matchCase, regex);
                        addTitleResults(root, resultsModel, titleItem, titleResults, arg.m_name);
                    }
                    // TODO: Find actual results
                    dialog.setValue(dialog.value() + 1);
                },
        }, *model->nodeData(root));
    }

    std::vector<std::pair<int, int>>
    FindWidget::find(QString const& target, QString const& searchPhrase, bool matchCase, bool regex) noexcept
    {
        std::vector<std::pair<int, int>> results;

        if (regex) {
            QRegularExpression re(searchPhrase);
            if (!matchCase)
                re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

            QRegularExpressionMatchIterator iter = re.globalMatch(target);
            while (iter.hasNext()) {
                QRegularExpressionMatch match = iter.next();
                results.emplace_back(match.capturedStart(), match.capturedEnd());
            }
        }
        else {
            for (int from = 0;
                 (from = target.indexOf(searchPhrase, from, matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive)) != -1;
                 from += searchPhrase.length())
                results.emplace_back(from, from + searchPhrase.length());
        }

        return results;
    }

    void
    FindWidget::addTitleResults(QModelIndex idx, QStandardItemModel& resultsModel, QStandardItem* resultModelParent,
            std::vector<std::pair<int, int>> const& results, QString const& title) noexcept
    {
        for (auto const& r : results) {
            QString searchResult = formatResult(r, title);
            auto* item = new QStandardItem(searchResult);
            item->setData(QPersistentModelIndex(idx), ModelIndex);
            resultModelParent->appendRow(item);
        }
    }

    QString FindWidget::formatResult(std::pair<int, int> const& result, QString const& str) noexcept
    {
        constexpr int maxChar = 10;
        const int start = std::max(0, result.first - maxChar);
        const int end = std::min(str.length(), result.second + maxChar);
        QString formatted;
        if (start > 0)
            formatted += "…";
        formatted += str.mid(start, result.first - start).toHtmlEscaped();
        formatted += "<b>";
        formatted += str.mid(result.first, result.second - result.first).toHtmlEscaped();
        formatted += "</b>";
        formatted += str.mid(result.second, end - result.second).toHtmlEscaped();
        if (end < str.length())
            formatted += "…";
        return formatted;
    }

    bool FindWidget::removeEmptyResults(QStandardItem* root) noexcept
    {
        if (root->data(ModelIndex).isValid())
            return true;
        if (root->hasChildren()) {
            bool hasResult = false;
            for (int i = 0; i < root->rowCount(); ++i) {
                if (removeEmptyResults(root->child(i)))
                    hasResult = true;
                else
                    root->removeRow(i--);
            }
            if (hasResult)
                return true;
        }
        return false;
    }

    void FindWidget::onSearchStarted()
    {
        QString searchTerm = m_ui->lineEditFind->text();
        auto[model, idx] = getSearchModelRoot();

        if (model == nullptr || !idx.isValid() || m_ui->lineEditFind->text().isEmpty())
            return;

        QItemSelectionModel* m = m_ui->treeView->selectionModel();
        m_ui->treeView->setModel(nullptr);
        m_ui->treeView->setItemDelegate(nullptr);
        delete m;
        m_findModel = std::make_unique<QStandardItemModel>();
        m_findModel->setColumnCount(1);

        QProgressDialog progress(tr("Looking for matches..."), tr("Abort"), 0, 1, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.setMinimumDuration(1000); // Don't show dialog if finished in less than 1 second

        search(model, idx, *m_findModel, m_findModel->invisibleRootItem(), progress);
        removeEmptyResults(m_findModel->invisibleRootItem());

        progress.setValue(progress.maximum());

        m_ui->treeView->setModel(m_findModel.get());
        m_ui->treeView->setItemDelegate(new internal::HtmlItemDelegate);
        m_ui->treeView->expandAll();
    }

    namespace internal {
        void
        HtmlItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
        {
            auto options = option;
            initStyleOption(&options, index);

            painter->save();

            QTextDocument doc;
            doc.setHtml(options.text);

            options.text = "";
            options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

            QSize iconSize = options.icon.actualSize(options.rect.size());
            painter->translate(options.rect.left() + iconSize.width(), options.rect.top());
            QRect clip(0, 0, options.rect.width() + iconSize.width(), options.rect.height());

            painter->setClipRect(clip);
            QAbstractTextDocumentLayout::PaintContext ctx;
            ctx.clip = clip;
            doc.documentLayout()->draw(painter, ctx);

            painter->restore();
        }

        QSize HtmlItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
        {
            auto options = option;
            initStyleOption(&options, index);

            QTextDocument doc;
            doc.setHtml(options.text);
            doc.setTextWidth(options.rect.width());
            return QSize(doc.idealWidth(), doc.size().height());
        }
    }
}