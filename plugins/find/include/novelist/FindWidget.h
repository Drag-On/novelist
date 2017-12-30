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

    protected:
        void changeEvent(QEvent* event) override;

    private:
        enum Role {
            ModelIndexRole = Qt::UserRole + 1, // Model index of result in project model
            ExcludedRole, // Flag indicating whether the item has been marked as excluded
            TypeRole, // Result type, title or content
            FindResultRole, // Result span
            MatchRole, // Matching string
        };
        enum ResultType {
            Title,
            Content,
        };

        void setupConnections() noexcept;

        std::pair<ProjectModel*, QModelIndex> getSearchModelRoot() noexcept;

        void
        search(ProjectModel* model, QModelIndex root, QStandardItemModel& resultsModel, QStandardItem* resultModelRoot,
                QProgressDialog& dialog) noexcept;

        std::vector<std::pair<int, int>>
        find(QString const& target, QString const& searchPhrase, bool matchCase, bool regex) noexcept;

        void addResults(QModelIndex idx, QStandardItemModel& resultsModel, QStandardItem* resultModelParent,
                std::vector<std::pair<int, int>> const& results, QString const& title, ResultType type) noexcept;

        QString formatResult(std::pair<int, int> const& result, QString const& str) noexcept;

        bool removeEmptyResults(QStandardItem* root) noexcept;

        bool removeEmptyResultsUp(QStandardItem* leaf) noexcept;

        void excludeItem(QStandardItem* item, bool exclude, bool checkParent = true, bool recursive = true) noexcept;

        bool replaceItem(QModelIndex idx) noexcept;

        void reset() noexcept;

    private slots:

        void onFindTextChanged(QString const& text);

        void onSearchStarted();

        void onPrevious();

        void onNext();

        void onExcludeItem();

        void onReplaceItem();

        void onReplaceAll();

        void onSelectionChanged(QItemSelection const& selected, QItemSelection const& deselected);

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
