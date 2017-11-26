/**********************************************************
 * @file   StatsDialog.cpp
 * @author jan
 * @date   11/25/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "StatsDialog.h"
#include <QtCore/QEvent>
#include <QtCharts/QChartView>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QtWidgets/QVBoxLayout>
#include "ui_StatsDialog.h"

QT_CHARTS_USE_NAMESPACE

namespace novelist {
    StatsDialog::StatsDialog(gsl::not_null<ProjectModel*> project, QWidget* parent, Qt::WindowFlags f)
            :QDialog(parent, f),
             m_ui{std::make_unique<Ui::StatsDialog>()},
             m_project(project)
    {
        m_ui->setupUi(this);

        auto data = readData();
        auto wordsSeries = std::make_unique<QLineSeries>();
        auto lettersSeries = std::make_unique<QLineSeries>();
        for (auto const& row : data) {
            wordsSeries->append(row.m_timeStamp.toMSecsSinceEpoch(), row.m_stats.m_wordCount);
            lettersSeries->append(row.m_timeStamp.toMSecsSinceEpoch(), row.m_stats.m_charCount);
        }

        createWordsChart(std::move(wordsSeries));
        createLettersChart(std::move(lettersSeries));
    }

    StatsDialog::~StatsDialog() = default;

    void StatsDialog::retranslateUi()
    {
        m_ui->retranslateUi(this);
    }

    void StatsDialog::changeEvent(QEvent* event)
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

    std::vector<StatDataRow> StatsDialog::readData() const noexcept
    {
        std::vector<StatDataRow> data;

        QString filename = m_project->saveDir().path() + QDir::separator() + ProjectStatCollector::filename();
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly))
            qWarning() << "Unable to read stats." << file.errorString();

        while (!file.atEnd()) {
            QByteArray line = file.readLine();
            auto tokens = line.split(',');
            StatDataRow row;
            row.m_timeStamp = QDateTime::fromSecsSinceEpoch(tokens.at(0).trimmed().toLongLong());
            row.m_stats.m_charCount = tokens.at(1).trimmed().toULongLong();
            row.m_stats.m_wordCount = tokens.at(2).trimmed().toULongLong();
            data.emplace_back(std::move(row));
        }

        return data;
    }

    void StatsDialog::createWordsChart(std::unique_ptr<QLineSeries> data) noexcept
    {
        auto chart = std::make_unique<QChart>();
        auto* series = data.release();
        chart->addSeries(series);
        chart->legend()->hide();
        chart->setTitle(tr("Number of words over time"));

        auto axisX = std::make_unique<QDateTimeAxis>();
        axisX->setFormat("dd.MM.yyyy\nhh:mm:ss");
        axisX->setTitleText(tr("Date"));
        auto* axisXPtr = axisX.release();
        chart->addAxis(axisXPtr, Qt::AlignBottom);
        series->attachAxis(axisXPtr);

        auto axisY = std::make_unique<QValueAxis>();
        axisY->setLabelFormat("%i");
        axisY->setTitleText(tr("# Words"));
        auto* axisYPtr = axisY.release();
        chart->addAxis(axisYPtr, Qt::AlignLeft);
        series->attachAxis(axisYPtr);

        auto chartView = std::make_unique<QChartView>(chart.release());
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setRubberBand(QChartView::HorizontalRubberBand);

        auto layout = new QVBoxLayout(m_ui->tabWordCount);
        layout->addWidget(chartView.release());
    }

    void StatsDialog::createLettersChart(std::unique_ptr<QLineSeries> data) noexcept
    {
        auto chart = std::make_unique<QChart>();
        auto* series = data.release();
        chart->addSeries(series);
        chart->legend()->hide();
        chart->setTitle(tr("Number of letters over time"));

        auto axisX = std::make_unique<QDateTimeAxis>();
        axisX->setFormat("dd.MM.yyyy\nhh:mm:ss");
        axisX->setTitleText(tr("Date"));
        auto* axisXPtr = axisX.release();
        chart->addAxis(axisXPtr, Qt::AlignBottom);
        series->attachAxis(axisXPtr);

        auto axisY = std::make_unique<QValueAxis>();
        axisY->setLabelFormat("%i");
        axisY->setTitleText(tr("# Letters"));
        auto* axisYPtr = axisY.release();
        chart->addAxis(axisYPtr, Qt::AlignLeft);
        series->attachAxis(axisYPtr);

        auto chartView = std::make_unique<QChartView>(chart.release());
        chartView->setRenderHint(QPainter::Antialiasing);
        chartView->setRubberBand(QChartView::HorizontalRubberBand);

        auto layout = new QVBoxLayout(m_ui->tabCharCount);
        layout->addWidget(chartView.release());
    }
}