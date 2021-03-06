/**********************************************************
 * @file   ProjectStatCollector.h
 * @author jan
 * @date   11/19/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_PROJECTSTATCOLLECTOR_H
#define NOVELIST_PROJECTSTATCOLLECTOR_H

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QFuture>
#include <QtCore/QDateTime>
#include <vector>
#include <model/ProjectModel.h>
#include <util/ConnectionWrapper.h>
#include <QtCore/QFutureWatcher>
#include "TextAnalyzer.h"

namespace novelist {

    struct StatDataRow;
    class StatsPlugin;

    namespace internal {
        struct Node {
            QString m_name;
            QString m_text;
            std::vector<Node> m_children;
        };

        struct AnalysisJob {
            QDateTime m_timeStamp;
            std::vector<Node> m_nodes;
        };

        StatDataRow analyze(AnalysisJob job) noexcept;
    }

    struct StatDataRow {
        QDateTime m_timeStamp;
        TextAnalyzer::Stats m_stats;
    };

    class ProjectStatCollector : public QObject {
    Q_OBJECT

    public:
        ProjectStatCollector() noexcept;

        ~ProjectStatCollector() noexcept override;

        /**
         * Set the watch interval
         * @param minutes Interval in minutes
         */
        void setWatchInterval(int minutes) noexcept;

        /*
         * @return The watch interval in minutes
         */
        int getWatchInterval() const noexcept;

        /**
         * @return Name of the stats file
         */
        static char const* filename() noexcept;

    public slots:

        void onProjectAboutToChange(ProjectModel* oldModel);

        void onProjectChanged(ProjectModel* model);

    private slots:

        void onTimeOut();

        void onFutureReady();

        void onProjectSaved(QDir const& saveDir);

    private:
        void setupWatcher() noexcept;

        internal::AnalysisJob makeJob() const noexcept;

        std::vector<internal::Node> buildNodeTree(QModelIndex const& root) const noexcept;

        void storeResults(StatDataRow result) noexcept;

        ProjectModel* m_model = nullptr;
        QTimer m_timer;
        int m_watchInterval = 900000; // 15 minutes interval
        ConnectionWrapper m_timeoutConnection;
        ConnectionWrapper m_projectSavedConnection;
        QFuture<StatDataRow> m_future;
        QFutureWatcher<StatDataRow> m_futureWatcher;
        std::vector<StatDataRow> m_dataPoints;

        inline static constexpr const char s_filename[] = "stats.csv";

        friend StatsPlugin;
    };
}

#endif //NOVELIST_PROJECTSTATCOLLECTOR_H
