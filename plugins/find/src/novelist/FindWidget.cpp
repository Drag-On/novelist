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
#include <QtWidgets/QMessageBox>
#include "FindWidget.h"
#include "ui_FindWidget.h"

namespace novelist {
    FindWidget::FindWidget(QWidget* parent) noexcept
            :QWidget(parent),
             m_ui(std::make_unique<Ui::FindWidget>())
    {
        m_ui->setupUi(this);
        setupConnections();

        m_mainWin = nullptr;
        for (QWidget* w : qApp->topLevelWidgets()) {
            auto* window = dynamic_cast<MainWindow*>(w);
            if (window) {
                m_mainWin = window;
                break;
            }
        }

        if (m_mainWin != nullptr)
            connect(m_mainWin, &MainWindow::projectChanged, [this](ProjectModel*) { reset(); });
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
        connect(m_ui->lineEditFind, &QLineEdit::textChanged, this, &FindWidget::onFindTextChanged);
        connect(m_ui->pushButtonSearch, &QPushButton::pressed, this, &FindWidget::onSearchStarted);
        connect(m_ui->pushButtonPrev, &QPushButton::pressed, this, &FindWidget::onPrevious);
        connect(m_ui->pushButtonNext, &QPushButton::pressed, this, &FindWidget::onNext);
        connect(m_ui->pushButtonExclude, &QPushButton::pressed, this, &FindWidget::onExcludeItem);
        connect(m_ui->pushButtonReplace, &QPushButton::pressed, this, &FindWidget::onReplaceItem);
        connect(m_ui->pushButtonReplaceAll, &QPushButton::pressed, this, &FindWidget::onReplaceAll);
    }

    std::pair<ProjectModel*, QModelIndex> FindWidget::getSearchModelRoot() noexcept
    {
        std::pair<ProjectModel*, QModelIndex> result;

        if (m_mainWin == nullptr)
            return result;

        result.first = m_mainWin->project();
        if (result.first == nullptr)
            return result;

        switch (m_ui->comboBoxScope->currentIndex()) {
            case 0: // Scene
            {
                auto[m, idx] = m_mainWin->sceneTabWidget()->current();
                if (m == result.first) {
                    result.second = idx;
                    return result;
                }
                break;
            }
            case 1: // Chapter
            {
                auto[m, idx] = m_mainWin->sceneTabWidget()->current();
                if (m == result.first) {
                    result.second = idx.parent();
                    return result;
                }
                break;
            }
            case 2: // Project
            {
                result.second = QModelIndex();
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

        bool const matchCase = m_ui->checkBoxMatchCase->isChecked();
        bool const regex = m_ui->checkBoxRegEx->isChecked();
        bool const searchTitles = m_ui->checkBoxSearchTitles->isChecked();
        QString const searchPhrase = m_ui->lineEditFind->text();
        QString const titleHtml = "<i>" + QCoreApplication::translate("FindWidget", "Title") + "</i>";
        QString const contentHtml = "<i>" + QCoreApplication::translate("FindWidget", "Content") + "</i>";

        using ProjectHeadData = ProjectModel::ProjectHeadData;
        using NotebookHeadData = ProjectModel::NotebookHeadData;
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
                [&](NotebookHeadData& arg) {
                    QStandardItem* item = new QStandardItem(QIcon(":/icons/node-notebook"), QCoreApplication::translate("novelist::ProjectModel", "Notebook"));
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
                        QStandardItem* titleItem = new QStandardItem(titleHtml);
                        item->appendRow(titleItem);
                        auto titleResults = find(arg.m_name, searchPhrase, matchCase, regex);
                        addResults(root, resultsModel, titleItem, titleResults, arg.m_name, ResultType::Title);
                    }
                    dialog.setValue(dialog.value() + 1);
                    for (int i = 0; i < childCount; ++i)
                        search(model, root.child(i, 0), resultsModel, item, dialog);
                },
                [&](SceneData& arg) {
                    QStandardItem* item = new QStandardItem(QIcon(":/icons/node-scene"), arg.m_name.toHtmlEscaped());
                    resultModelRoot->appendRow(item);
                    if (searchTitles) {
                        QStandardItem* titleItem = new QStandardItem(titleHtml);
                        item->appendRow(titleItem);
                        auto titleResults = find(arg.m_name, searchPhrase, matchCase, regex);
                        addResults(root, resultsModel, titleItem, titleResults, arg.m_name, ResultType::Title);
                    }
                    QStandardItem* contentItem = new QStandardItem(contentHtml);
                    item->appendRow(contentItem);
                    QString text = arg.m_doc->toRawText();
                    auto contentResults = find(text, searchPhrase, matchCase, regex);
                    addResults(root, resultsModel, contentItem, contentResults, text, ResultType::Content);
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
    FindWidget::addResults(QModelIndex idx, QStandardItemModel& resultsModel, QStandardItem* resultModelParent,
            std::vector<std::pair<int, int>> const& results, QString const& title, ResultType type) noexcept
    {
        for (auto const& r : results) {
            QString searchResult = formatResult(r, title);
            auto* item = new QStandardItem(searchResult);
            item->setData(QPersistentModelIndex(idx), ModelIndexRole);
            item->setData(type, TypeRole);
            item->setData(QVariant::fromValue(r), FindResultRole);
            item->setData(title.mid(r.first, r.second - r.first), MatchRole);
            resultModelParent->appendRow(item);
        }
    }

    QString FindWidget::formatResult(std::pair<int, int> const& result, QString const& str) noexcept
    {
        constexpr int maxChar = 15;
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
        if (root->data(ModelIndexRole).isValid())
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

    bool FindWidget::removeEmptyResultsUp(QStandardItem* leaf) noexcept
    {
        if (leaf->rowCount() == 0) {
            auto parent = leaf->parent();
            if (parent) {
                parent->removeRow(leaf->row());
                removeEmptyResultsUp(parent);
                return true;
            }
        }
        return false;
    }

    void FindWidget::excludeItem(QStandardItem* item, bool exclude, bool checkParent, bool recursive) noexcept
    {
        bool isExcluded = item->data(ExcludedRole).toBool();
        item->setData(exclude, ExcludedRole);
        QString text = item->text();
        if (exclude && !isExcluded)
            text = "<s>" + text + "</s>";
        else if (!exclude && isExcluded)
            text = text.mid(3, text.length() - 7);
        item->setText(text);

        if (recursive) {
            for (int i = 0; i < item->rowCount(); ++i)
                excludeItem(item->child(i, 0), exclude, false);
        }

        if (checkParent && item->parent() != nullptr) {
            auto parent = item->parent();
            bool allExcluded = true;
            for (int i = 0; i < parent->rowCount(); ++i)
                if (!parent->child(i, 0)->data(ExcludedRole).toBool()) {
                    allExcluded = false;
                    break;
                }
            if (allExcluded && !parent->data(ExcludedRole).toBool())
                excludeItem(parent, true, true, false);
            else if (!allExcluded && parent->data(ExcludedRole).toBool())
                excludeItem(parent, false, true, false);
        }
    }

    bool FindWidget::replaceItem(QModelIndex idx) noexcept
    {
        ProjectModel* model = getSearchModelRoot().first;
        auto const modelIdx = qvariant_cast<QModelIndex>(idx.data(ModelIndexRole));
        auto const type = static_cast<ResultType>(qvariant_cast<int>(idx.data(TypeRole)));
        auto const findResult = qvariant_cast<std::pair<int, int>>(idx.data(FindResultRole));
        auto const match = idx.data(MatchRole).toString();
        QString const replace = m_ui->lineEditReplace->text();

        if (!modelIdx.isValid())
            return false;

        using ProjectHeadData = ProjectModel::ProjectHeadData;
        using SceneData = ProjectModel::SceneData;
        using ChapterData = ProjectModel::ChapterData;

        bool success = true;
        std::visit(Overloaded {
                [](auto&) { qWarning() << "Can't replace in invalid node type."; },
                [&](ChapterData& arg) {
                    QString name = arg.m_name;
                    if (name.mid(findResult.first, findResult.second - findResult.first) != match) {
                        success = false;
                        return;
                    }
                    name.replace(findResult.first, findResult.second - findResult.first, replace);
                    model->setData(modelIdx, name, Qt::EditRole);
                },
                [&](SceneData& arg) {
                    if (type == ResultType::Title) {
                        QString name = arg.m_name;
                        if (name.mid(findResult.first, findResult.second - findResult.first) != match) {
                            success = false;
                            return;
                        }
                        name.replace(findResult.first, findResult.second - findResult.first, replace);
                        model->setData(modelIdx, name, Qt::EditRole);
                    }
                    else {
                        QTextCursor cursor(arg.m_doc.get());
                        cursor.setPosition(findResult.first);
                        cursor.setPosition(findResult.second, QTextCursor::MoveMode::KeepAnchor);
                        if (cursor.selectedText() != match) {
                            success = false;
                            return;
                        }
                        cursor.insertText(replace);
                    }
                },
        }, *model->nodeData(modelIdx));

        if (int diff = replace.length() - match.length(); success && diff != 0) {
            auto parent = idx.parent();
            if (parent.isValid()) {
                for (int i = 0; i < m_findModel->rowCount(parent); ++i) {
                    auto childIdx = m_findModel->index(i, 0, parent);
                    auto r = qvariant_cast<std::pair<int, int>>(childIdx.data(FindResultRole));
                    if (r.first > findResult.first) {
                        r.first += diff;
                        r.second += diff;
                        m_findModel->setData(childIdx, QVariant::fromValue(r), FindResultRole);
                    }
                }
            }
        }

        return success;
    }

    void FindWidget::reset() noexcept
    {
        this->m_ui->pushButtonExclude->setEnabled(false);
        this->m_ui->pushButtonReplace->setEnabled(false);

        QItemSelectionModel* m = this->m_ui->treeView->selectionModel();
        this->m_ui->treeView->setModel(nullptr);
        this->m_ui->treeView->setItemDelegate(nullptr);
        delete m;

        m_findModel.reset();
    }

    void FindWidget::onFindTextChanged(QString const& text)
    {
        m_ui->pushButtonSearch->setEnabled(!text.isEmpty());
    }

    void FindWidget::onSearchStarted()
    {
        QString searchTerm = m_ui->lineEditFind->text();
        auto[model, idx] = getSearchModelRoot();

        reset();
        m_findModel = std::make_unique<QStandardItemModel>();
        m_findModel->setColumnCount(1);

        if (model == nullptr || m_ui->lineEditFind->text().isEmpty())
            return;

        QProgressDialog progress(tr("Looking for matches..."), tr("Abort"), 0, 1, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.setMinimumDuration(1000); // Don't show dialog if finished in less than 1 second

        if (!idx.isValid()) { // Invisible root, consider all its children
            for (int i = 0; i < model->rowCount(QModelIndex()); ++i)
                search(model, model->index(i, 0, QModelIndex()), *m_findModel, m_findModel->invisibleRootItem(), progress);
        }
        else
            search(model, idx, *m_findModel, m_findModel->invisibleRootItem(), progress);
        removeEmptyResults(m_findModel->invisibleRootItem());

        progress.setValue(progress.maximum());

        m_ui->treeView->setModel(m_findModel.get());
        m_ui->treeView->setItemDelegate(new internal::HtmlItemDelegate);
        m_ui->treeView->expandAll();
        connect(m_ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &FindWidget::onSelectionChanged);
    }

    void FindWidget::onPrevious()
    {
        QModelIndex origIdx = m_ui->treeView->currentIndex();
        QModelIndex idx, lastIdx;
        do {
            lastIdx = idx;
            idx = m_ui->treeView->moveCursor(internal::InternalTreeView::CursorAction::MovePrevious, Qt::NoModifier);
            m_ui->treeView->expand(idx);
            idx = m_ui->treeView->moveCursor(internal::InternalTreeView::CursorAction::MovePrevious, Qt::NoModifier);
            m_ui->treeView->setCurrentIndex(idx);
        } while (idx != lastIdx && !idx.data(ModelIndexRole).isValid());
        if (idx == lastIdx) // If we hit the top retain topmost selected result instead of jumping to tree root
            m_ui->treeView->setCurrentIndex(origIdx);
    }

    void FindWidget::onNext()
    {
        QModelIndex idx, lastIdx;
        do {
            lastIdx = idx;
            m_ui->treeView->expand(m_ui->treeView->currentIndex());
            idx = m_ui->treeView->moveCursor(internal::InternalTreeView::CursorAction::MoveNext, Qt::NoModifier);
            m_ui->treeView->setCurrentIndex(idx);
        } while (idx != lastIdx && !idx.data(ModelIndexRole).isValid());

    }

    void FindWidget::onExcludeItem()
    {
        Expects(!m_ui->treeView->selectionModel()->selectedIndexes().isEmpty());

        auto idx = m_ui->treeView->selectionModel()->selectedIndexes().front();
        auto item = m_findModel->itemFromIndex(idx);
        bool isExcluded = !item->data(ExcludedRole).toBool();
        excludeItem(item, isExcluded);
    }

    void FindWidget::onReplaceItem()
    {
        Expects(!m_ui->treeView->selectionModel()->selectedIndexes().isEmpty());
        Expects(m_ui->treeView->selectionModel()->selectedIndexes().front().data(ModelIndexRole).isValid());

        auto idx = m_ui->treeView->selectionModel()->selectedIndexes().front();
        if (!replaceItem(idx)) {
            QMessageBox msgBox;
            msgBox.setText(tr("Unable to replace the selected occurence."));
            msgBox.setInformativeText(tr("The project might have changed since this result was found."));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.setIcon(QMessageBox::Information);
            msgBox.exec();
        }
        else {
            onNext();
            auto parentItem = m_findModel->itemFromIndex(idx.parent());
            m_findModel->removeRow(idx.row(), parentItem->index());
            removeEmptyResultsUp(parentItem);
        }
    }

    void FindWidget::onReplaceAll()
    {
        m_ui->treeView->setCurrentIndex(QModelIndex());
        onNext();

        if (!m_ui->treeView->currentIndex().isValid())
            return;

        int numFailed = 0;
        QModelIndex idx, lastIdx;
        while (true) {
            lastIdx = idx;
            idx = m_ui->treeView->currentIndex();
            if (idx != lastIdx && idx.isValid() && idx.data(ModelIndexRole).isValid()) {
                if (!idx.data(ExcludedRole).toBool()) {
                    if (!replaceItem(idx)) {
                        ++numFailed;
                        onNext();
                    }
                    else {
                        onNext();
                        m_findModel->removeRow(idx.row(), idx.parent());
                        idx = QModelIndex();
                    }
                }
                else
                    onNext();
            }
            else
                break;
        }

        removeEmptyResults(m_findModel->invisibleRootItem());

        if (numFailed > 0) {
            QMessageBox msgBox;
            msgBox.setText(tr("%1 occurences could not be replaced.").arg(numFailed));
            msgBox.setInformativeText(tr("The project might have changed since the results were found."));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.setIcon(QMessageBox::Information);
            msgBox.exec();
        }
    }

    void FindWidget::onSelectionChanged(QItemSelection const& selected, QItemSelection const& deselected)
    {
        m_ui->pushButtonExclude->setEnabled(!selected.isEmpty());
        m_ui->pushButtonReplace->setEnabled(!selected.isEmpty() && selected.front().indexes().front().data(ModelIndexRole).isValid());
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