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
#include <memory>
#include <model/ProjectModel.h>

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
        void setupConnections() noexcept;

        std::pair<ProjectModel*, QModelIndex> getSearchModelRoot() noexcept;

        void search(ProjectModel* model, QModelIndex root, QAbstractItemModel& resultsModel) noexcept;

    private slots:
        void onSearchStarted();

        std::unique_ptr<Ui::FindWidget> m_ui;
    };
}

#endif //NOVELIST_FINDWIDGET_H
