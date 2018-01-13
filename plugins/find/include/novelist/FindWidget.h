/**********************************************************
 * @file   FindWidget.h
 * @author jan
 * @date   12/17/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_FINDWIDGET_H
#define NOVELIST_FINDWIDGET_H

#include <QtWidgets/QWidget>
#include <QStandardItemModel>
#include <QProgressDialog>
#include <memory>
#include <model/ProjectModel.h>
#include <QtWidgets/QStyledItemDelegate>

namespace Ui {
    class FindWidget;
}

namespace novelist {
    class FindWidget : public QWidget {
    public:
        explicit FindWidget(QWidget* parent = nullptr) noexcept;

        void retranslateUi() noexcept;

    protected:
        void changeEvent(QEvent* event) override;

    private:
        enum Role {
            ModelIndexRole = Qt::UserRole + 1, // Model index of result in project model
            NodeTypeRole, // NodeType
            ResultSpanRole, // Result span
            MatchRole, // Matching string
            CountRole, // Amount of occurrences in subtree
            IncludedCountRole, // Amount of included occurrences in subtree
            StaticTextRole, // Static display text
        };
        enum NodeType {
            ProjectRoot,
            NotebookRoot,
            Chapter,
            Scene,
            TitleResultTopic,
            ContentResultTopic,
            Result,
        };
        enum ResultType {
            None,
            Title,
            Content,
        };

        void setupConnections() noexcept;

        QModelIndex getSearchModelRoot() noexcept;

        std::unique_ptr<QStandardItem> makeNode(NodeType type, QModelIndex idx = QModelIndex(), QString const& staticText = "") const noexcept;

        void
        search(ProjectModel* model, QModelIndex root, QStandardItemModel& resultsModel, QStandardItem* resultModelRoot,
                QProgressDialog& dialog) noexcept;

        std::vector<std::pair<int, int>>
        find(QString const& target, QString const& searchPhrase, bool matchCase, bool regex) noexcept;

        void addResults(QModelIndex idx, QStandardItem* resultModelParent,
                std::vector<std::pair<int, int>> const& results, QString const& title) noexcept;

        QString composeResultString(std::pair<int, int> const& result, QString const& str) noexcept;

        QString formatResult(QStandardItem* item) noexcept;

        QString formatNonResult(QStandardItem* item) noexcept;

        void reformatNode(QStandardItem* item) noexcept;

        void removeRow(QStandardItem* parent, int row) noexcept;

        bool removeEmptyResults(QStandardItem* root) noexcept;

        bool removeEmptyResultsUp(QStandardItem* leaf) noexcept;

        void updateCountsAndTitles(QStandardItem* root) noexcept;

        void excludeItem(QStandardItem* item, bool exclude) noexcept;

        bool isExcluded(QStandardItem* item) const noexcept;

        void updateExcludeButtonText() noexcept;

        bool replaceItem(QModelIndex idx) noexcept;

        void reset() noexcept;

        ResultType getResultType(QStandardItem* item) const noexcept;

        bool isResultNode(QStandardItem* item) const noexcept;

        void selectInMatches(QModelIndex const& idx) noexcept;

    private slots:

        void onFindTextChanged(QString const& text);

        void onSearchStarted();

        void onPrevious();

        void onNext();

        void onExcludeItem();

        void onReplaceItem();

        void onReplaceAll();

        void onSelectionChanged(QItemSelection const& selected, QItemSelection const& deselected);

        void onItemActivated(QModelIndex const& index);

        void onDataChanged(QModelIndex const& topLeft, QModelIndex const& bottomRight, QVector<int> const& roles);

        std::unique_ptr<Ui::FindWidget> m_ui;
        std::unique_ptr<QStandardItemModel> m_findModel;
        MainWindow* m_mainWin = nullptr;
    };

    namespace internal {
        class HtmlItemDelegate : public QStyledItemDelegate {
        protected:
            void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

            QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
        };
    }
}

#endif //NOVELIST_FINDWIDGET_H
