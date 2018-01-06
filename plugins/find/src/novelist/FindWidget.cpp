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
#include <stack>
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
        connect(m_ui->treeView, &QTreeView::activated, this, &FindWidget::onItemActivated);
    }

    QModelIndex FindWidget::getSearchModelRoot() noexcept
    {
        if (m_mainWin == nullptr || m_mainWin->project() == nullptr)
            return QModelIndex();

        switch (m_ui->comboBoxScope->currentIndex()) {
            case 0: // Scene
            {
                auto[m, idx] = m_mainWin->sceneTabWidget()->current();
                if (m == m_mainWin->project())
                    return idx;
                break;
            }
            case 1: // Chapter
            {
                auto[m, idx] = m_mainWin->sceneTabWidget()->current();
                if (m == m_mainWin->project())
                    return idx.parent();
                break;
            }
            case 2: // Project
            {
                return m_mainWin->project()->projectRootIndex().parent();
            }
            default:
                return QModelIndex();
        }
        return QModelIndex();
    }

    std::unique_ptr<QStandardItem>
    FindWidget::makeNode(NodeType type, QModelIndex idx, QString const& staticText) const noexcept
    {
        auto node = std::make_unique<QStandardItem>();
        node->setData(type, NodeTypeRole);
        node->setData(QPersistentModelIndex(idx), ModelIndexRole);
        node->setData(0, CountRole);
        node->setData(0, IncludedCountRole);
        node->setData(staticText, StaticTextRole);
        node->setData(staticText, Qt::DisplayRole);
        switch (type) {
            case NodeType::ProjectRoot: {
                node->setIcon(QIcon(":/icons/node-project"));
                break;
            }
            case NodeType::NotebookRoot: {
                QString const text = QCoreApplication::translate("novelist::ProjectModel", "Notebook");
                node->setIcon(QIcon(":/icons/node-notebook"));
                node->setData(text, StaticTextRole);
                node->setData(text, Qt::DisplayRole);
                break;
            }
            case NodeType::Chapter: {
                node->setIcon(QIcon(":/icons/node-chapter"));
                break;
            }
            case NodeType::Scene: {
                node->setIcon(QIcon(":/icons/node-scene"));
                break;
            }
            case NodeType::TitleResultTopic: {
                QString const text = "<i>" + QCoreApplication::translate("FindWidget", "Title") + "</i>";
                node->setData(text, StaticTextRole);
                node->setData(text, Qt::DisplayRole);
                break;
            }
            case NodeType::ContentResultTopic: {
                QString const text = "<i>" + QCoreApplication::translate("FindWidget", "Content") + "</i>";
                node->setData(text, StaticTextRole);
                node->setData(text, Qt::DisplayRole);
                break;
            }
            case NodeType::Result:
                node->setData(1, CountRole);
                node->setData(1, IncludedCountRole);
                break;
        }

        return node;
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

        using ProjectHeadData = ProjectModel::ProjectHeadData;
        using NotebookHeadData = ProjectModel::NotebookHeadData;
        using SceneData = ProjectModel::SceneData;
        using ChapterData = ProjectModel::ChapterData;

        std::visit(Overloaded {
                [](auto&) { qWarning() << "Can't search invalid node type."; },
                [&](ProjectHeadData& arg) {
                    QStandardItem* item = makeNode(NodeType::ProjectRoot, root, arg.m_properties.m_name.toHtmlEscaped()).release();
                    resultModelRoot->appendRow(item);
                    int const childCount = model->rowCount(root);
                    dialog.setMaximum(dialog.maximum() + childCount);
                    dialog.setValue(dialog.value() + 1);
                    for (int i = 0; i < childCount; ++i)
                        search(model, root.child(i, 0), resultsModel, item, dialog);
                },
                [&](NotebookHeadData& /*arg*/) {
                    QStandardItem* item = makeNode(NodeType::NotebookRoot, root).release();
                    resultModelRoot->appendRow(item);
                    int const childCount = model->rowCount(root);
                    dialog.setMaximum(dialog.maximum() + childCount);
                    dialog.setValue(dialog.value() + 1);
                    for (int i = 0; i < childCount; ++i)
                        search(model, root.child(i, 0), resultsModel, item, dialog);
                },
                [&](ChapterData& arg) {
                    QStandardItem* item = makeNode(NodeType::Chapter, root, arg.m_name.toHtmlEscaped()).release();
                    resultModelRoot->appendRow(item);
                    int const childCount = model->rowCount(root);
                    dialog.setMaximum(dialog.maximum() + childCount);
                    if (searchTitles) {
                        QStandardItem* titleItem = makeNode(NodeType::TitleResultTopic, root).release();
                        item->appendRow(titleItem);
                        auto titleResults = find(arg.m_name, searchPhrase, matchCase, regex);
                        addResults(root, titleItem, titleResults, arg.m_name);
                    }
                    dialog.setValue(dialog.value() + 1);
                    for (int i = 0; i < childCount; ++i)
                        search(model, root.child(i, 0), resultsModel, item, dialog);
                },
                [&](SceneData& arg) {
                    QStandardItem* item = makeNode(NodeType::Scene, root, arg.m_name.toHtmlEscaped()).release();
                    resultModelRoot->appendRow(item);
                    if (searchTitles) {
                        QStandardItem* titleItem = makeNode(NodeType::TitleResultTopic, root).release();
                        item->appendRow(titleItem);
                        auto titleResults = find(arg.m_name, searchPhrase, matchCase, regex);
                        addResults(root, titleItem, titleResults, arg.m_name);
                    }
                    QStandardItem* contentItem = makeNode(NodeType::ContentResultTopic, root).release();
                    item->appendRow(contentItem);
                    QString text = arg.m_doc->toRawText();
                    auto contentResults = find(text, searchPhrase, matchCase, regex);
                    addResults(root, contentItem, contentResults, text);
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
    FindWidget::addResults(QModelIndex idx, QStandardItem* resultModelParent,
            std::vector<std::pair<int, int>> const& results, QString const& title) noexcept
    {
        for (auto const& r : results) {
            QString searchResult = composeResultString(r, title);
            auto item = makeNode(NodeType::Result, idx, searchResult);
            item->setData(QVariant::fromValue(r), ResultSpanRole);
            item->setData(title.mid(r.first, r.second - r.first), MatchRole);
            resultModelParent->appendRow(item.release());
        }
    }

    QString FindWidget::composeResultString(std::pair<int, int> const& result, QString const& str) noexcept
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

    QString FindWidget::formatResult(QStandardItem* item) noexcept
    {
        int const includedCount = item->data(IncludedCountRole).toInt();

        QString displayText = item->data(StaticTextRole).toString();
        if (includedCount == 0)
            displayText = "<s>" + displayText + "</s>";

        return displayText;
    }

    QString FindWidget::formatNonResult(QStandardItem* item) noexcept
    {
        int const includedCount = item->data(IncludedCountRole).toInt();
        int const count = item->data(CountRole).toInt();

        QString displayText;
        if (includedCount == 0)
            displayText += "<s>";
        displayText += item->data(StaticTextRole).toString();
        if (includedCount == 0)
            displayText += "</s>";
        displayText += "   <i style=\"color:gray\">";
        displayText += tr("(%1 of %2 matches)")
                .arg(includedCount)
                .arg(count);
        displayText += "</i>";

        return displayText;
    }

    void FindWidget::reformatNode(QStandardItem* item) noexcept
    {
        if (isResultNode(item))
            item->setData(formatResult(item), Qt::DisplayRole);
        else
            item->setData(formatNonResult(item), Qt::DisplayRole);
    }

    void FindWidget::removeRow(QStandardItem* parent, int row) noexcept
    {
        auto child = parent->child(row, 0);
        int const countChange = -child->data(CountRole).toInt();
        int const includedCountChange = -child->data(IncludedCountRole).toInt();
        parent->removeRow(row);
        do {
            parent->setData(parent->data(CountRole).toInt() + countChange, CountRole);
            parent->setData(parent->data(IncludedCountRole).toInt() + includedCountChange, IncludedCountRole);
            parent = parent->parent();
        } while (parent != nullptr);
    }

    bool FindWidget::removeEmptyResults(QStandardItem* root) noexcept
    {
        if (isResultNode(root))
            return true;
        if (root->hasChildren()) {
            bool hasResult = false;
            for (int i = 0; i < root->rowCount(); ++i) {
                if (removeEmptyResults(root->child(i)))
                    hasResult = true;
                else
                    removeRow(root, i--);
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
                removeRow(parent, leaf->row());
                removeEmptyResultsUp(parent);
                return true;
            }
            else {
                m_findModel->removeRow(leaf->row(), QModelIndex());
                return true;
            }
        }
        qWarning() << "Tried to remove parent of non-empty node of find result.";
        return false;
    }

    void FindWidget::updateCountsAndTitles(QStandardItem* root) noexcept
    {
        int count = 0;
        int includedCount = 0;
        int const childCount = root->rowCount();
        for (int i = 0; i < childCount; ++i) {
            auto c = root->child(i, 0);
            if (!isResultNode(c))
                updateCountsAndTitles(c);
            count += c->data(CountRole).toInt();
            includedCount += c->data(IncludedCountRole).toInt();
        }
        root->setData(count, CountRole);
        root->setData(includedCount, IncludedCountRole);
        reformatNode(root);
    }

    void FindWidget::excludeItem(QStandardItem* item, bool exclude) noexcept
    {
        auto updateNode = [this](QStandardItem* item, bool exclude) {
            bool const excluded = isExcluded(item);
            if (excluded && !exclude) // include
                item->setData(item->data(CountRole), IncludedCountRole);
            else if (!excluded && exclude) // exclude
                item->setData(0, IncludedCountRole);
        };
        int const previousIncludedCount = item->data(IncludedCountRole).toInt();

        // Update self and children
        std::stack<QStandardItem*> queue;
        queue.push(item);
        while(!queue.empty()) {
            auto node = queue.top();
            queue.pop();
            updateNode(node, exclude);
            for (int i = 0; i < node->rowCount(); ++i)
                queue.push(node->child(i, 0));
        }

        int const includedCountDiff = item->data(IncludedCountRole).toInt() - previousIncludedCount;

        // Update parents
        auto parent = item->parent();
        while (parent != nullptr) {
            int const newParentIncludedCount = parent->data(IncludedCountRole).toInt() + includedCountDiff;
            parent->setData(newParentIncludedCount, IncludedCountRole);
            parent = parent->parent();
        }
    }

    bool FindWidget::isExcluded(QStandardItem* item) const noexcept
    {
        return item->data(IncludedCountRole).toInt() <= 0;
    }

    bool FindWidget::replaceItem(QModelIndex idx) noexcept
    {
        Expects(m_mainWin != nullptr);
        Expects(m_mainWin->project() != nullptr);

        ProjectModel* model = m_mainWin->project();
        auto const modelIdx = qvariant_cast<QModelIndex>(idx.data(ModelIndexRole));
        auto const resultType = getResultType(m_findModel->itemFromIndex(idx));
        auto const findResult = qvariant_cast<std::pair<int, int>>(idx.data(ResultSpanRole));
        auto const match = idx.data(MatchRole).toString();
        QString const replace = m_ui->lineEditReplace->text();

        if (!modelIdx.isValid())
            return false;

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
                    if (resultType == ResultType::Title) {
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
                    auto r = qvariant_cast<std::pair<int, int>>(childIdx.data(ResultSpanRole));
                    if (r.first > findResult.first) {
                        r.first += diff;
                        r.second += diff;
                        m_findModel->setData(childIdx, QVariant::fromValue(r), ResultSpanRole);
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

    FindWidget::ResultType FindWidget::getResultType(QStandardItem* item) const noexcept
    {
        if (item->parent() != nullptr) {
            if (item->parent()->data(NodeTypeRole).toInt() == TitleResultTopic)
                return ResultType::Title;
            else if (item->parent()->data(NodeTypeRole).toInt() == ContentResultTopic)
                return ResultType::Content;
        }
        return ResultType::None;
    }

    bool FindWidget::isResultNode(QStandardItem* item) const noexcept
    {
        return getResultType(item) != ResultType::None;
    }

    void FindWidget::onFindTextChanged(QString const& text)
    {
        m_ui->pushButtonSearch->setEnabled(!text.isEmpty());
    }

    void FindWidget::onSearchStarted()
    {
        Expects(m_mainWin != nullptr);

        QString searchTerm = m_ui->lineEditFind->text();
        auto model = m_mainWin->project();
        auto idx = getSearchModelRoot();

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
        updateCountsAndTitles(m_findModel->invisibleRootItem());

        progress.setValue(progress.maximum());

        m_ui->treeView->setModel(m_findModel.get());
        m_ui->treeView->setItemDelegate(new internal::HtmlItemDelegate);
        m_ui->treeView->expandAll();
        connect(m_ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &FindWidget::onSelectionChanged);
        connect(m_findModel.get(), &QStandardItemModel::dataChanged, this, &FindWidget::onDataChanged);
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
        } while (idx != lastIdx && !isResultNode(m_findModel->itemFromIndex(idx)));
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
        } while (idx != lastIdx && !isResultNode(m_findModel->itemFromIndex(idx)));

    }

    void FindWidget::onExcludeItem()
    {
        Expects(!m_ui->treeView->selectionModel()->selectedIndexes().isEmpty());

        auto idx = m_ui->treeView->selectionModel()->selectedIndexes().front();
        auto item = m_findModel->itemFromIndex(idx);
        excludeItem(item, !isExcluded(item));
    }

    void FindWidget::onReplaceItem()
    {
        Expects(!m_ui->treeView->selectionModel()->selectedIndexes().isEmpty());
        Expects(isResultNode(m_findModel->itemFromIndex(m_ui->treeView->selectionModel()->selectedIndexes().front())));

        auto idx = m_ui->treeView->selectionModel()->selectedIndexes().front();
        if (!replaceItem(idx)) {
            QMessageBox msgBox;
            msgBox.setText(tr("Unable to replace the selected occurrence."));
            msgBox.setInformativeText(tr("The project might have changed since this result was found."));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.setIcon(QMessageBox::Information);
            msgBox.exec();
        }
        else {
            onNext();
            auto parentItem = m_findModel->itemFromIndex(idx.parent());
            removeRow(parentItem, idx.row());
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
            if (idx != lastIdx && idx.isValid() && isResultNode(m_findModel->itemFromIndex(idx))) {
                if (!isExcluded(m_findModel->itemFromIndex(idx))) {
                    if (!replaceItem(idx)) {
                        ++numFailed;
                        onNext();
                    }
                    else {
                        onNext();
                        removeRow(m_findModel->itemFromIndex(idx.parent()), idx.row());
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
            msgBox.setText(tr("%1 occurrences could not be replaced.").arg(numFailed));
            msgBox.setInformativeText(tr("The project might have changed since the results were found."));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.setIcon(QMessageBox::Information);
            msgBox.exec();
        }
    }

    void FindWidget::onSelectionChanged(QItemSelection const& selected, QItemSelection const& /*deselected*/)
    {
        m_ui->pushButtonExclude->setEnabled(!selected.isEmpty());
        m_ui->pushButtonReplace->setEnabled(!selected.isEmpty() && isResultNode(m_findModel->itemFromIndex(selected.front().indexes().front())));
    }

    void FindWidget::onItemActivated(QModelIndex const& index)
    {
        if (!isResultNode(m_findModel->itemFromIndex(index)))
            return;

        if (m_mainWin == nullptr)
            return;

        SceneTabWidget* tabWidget = m_mainWin->sceneTabWidget();
        if (tabWidget == nullptr)
            return;

        ProjectModel* model = m_mainWin->project();
        if (model == nullptr)
            return;

        // Select correct node in project view
        auto idx = index.data(ModelIndexRole).toModelIndex();
        if (!idx.isValid())
            return;
        m_mainWin->projectView()->selectionModel()->select(idx, QItemSelectionModel::SelectCurrent);
        m_mainWin->projectView()->scrollTo(idx);

        // If this is from a scene, select the occurrence with cursor
        if (getResultType(m_findModel->itemFromIndex(index)) == ResultType::Content) {
            auto r = qvariant_cast<std::pair<int, int>>(index.data(ResultSpanRole));
            tabWidget->openScene(model, idx);
            TextEditor* edit = tabWidget->currentEditor();
            if (edit == nullptr)
                return;
            auto cursor = edit->textCursor();
            cursor.setPosition(r.first, QTextCursor::MoveMode::MoveAnchor);
            cursor.setPosition(r.second, QTextCursor::MoveMode::KeepAnchor);
            edit->setTextCursor(cursor);
        }
    }

    void FindWidget::onDataChanged(QModelIndex const& topLeft, QModelIndex const& bottomRight, QVector<int> const& roles)
    {
        // Reformat display text if significant data changed
        if (roles.empty() || roles.contains(StaticTextRole) || roles.contains(CountRole) || roles.contains(IncludedCountRole)) {
            auto const parent = topLeft.parent();
            for (int i = topLeft.row(); i <= bottomRight.row(); ++i) {
                auto const idx = m_findModel->index(i, 0, parent);
                auto const item = m_findModel->itemFromIndex(idx);
                if (item->data(StaticTextRole).isValid())
                    reformatNode(item);
            }
        }
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
            doc.setTextWidth(options.rect.width());

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
            return QSize(doc.size().width(), doc.size().height());
        }
    }
}