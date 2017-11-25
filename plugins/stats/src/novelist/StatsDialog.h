/**********************************************************
 * @file   StatsDialog.h
 * @author jan
 * @date   11/25/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_STATSDIALOG_H
#define NOVELIST_STATSDIALOG_H

#include <QtCore/QArgument>
#include <QtWidgets/QDialog>
#include <QtCharts/QLineSeries>
#include <memory>
#include <model/ProjectModel.h>
#include <ProjectStatCollector.h>

namespace Ui {
    class StatsDialog;
}

namespace novelist {
    class StatsDialog : public QDialog
    {
        Q_OBJECT

    public:
        /**
         * @param parent Parent widget
         * @param f Window flags
         */
        explicit StatsDialog(gsl::not_null<ProjectModel*> project, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags{});

        ~StatsDialog() noexcept override;

        /**
         * Translate UI to new language
         */
        void retranslateUi();

    protected:
        void changeEvent(QEvent* event) override;

    private:
        std::unique_ptr<Ui::StatsDialog> m_ui;
        ProjectModel* m_project;

        std::vector<StatDataRow> readData() const noexcept;

        void createWordsChart(std::unique_ptr<QT_CHARTS_NAMESPACE::QLineSeries> data) noexcept;

        void createLettersChart(std::unique_ptr<QT_CHARTS_NAMESPACE::QLineSeries> data) noexcept;
    };
}

#endif //NOVELIST_STATSDIALOG_H
