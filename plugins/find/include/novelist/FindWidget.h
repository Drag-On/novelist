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
            ModelIndex = Qt::UserRole + 1,
        };

        void setupConnections() noexcept;

        std::pair<ProjectModel*, QModelIndex> getSearchModelRoot() noexcept;

        void
        search(ProjectModel* model, QModelIndex root, QStandardItemModel& resultsModel, QStandardItem* resultModelRoot,
                QProgressDialog& dialog) noexcept;

        std::vector<std::pair<int, int>>
        find(QString const& target, QString const& searchPhrase, bool matchCase, bool regex) noexcept;

        void addTitleResults(QModelIndex idx, QStandardItemModel& resultsModel, QStandardItem* resultModelParent,
                std::vector<std::pair<int, int>> const& results, QString const& title) noexcept;

        QString formatResult(std::pair<int, int> const& result, QString const& str) noexcept;

        bool removeEmptyResults(QStandardItem* root) noexcept;

    private slots:

        void onSearchStarted();

        std::unique_ptr<Ui::FindWidget> m_ui;
        std::unique_ptr<QStandardItemModel> m_findModel;
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
