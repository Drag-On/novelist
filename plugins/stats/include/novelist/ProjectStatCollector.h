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
#include "TextAnalyzer.h"

namespace novelist {

    struct StatDataRow;

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
        ~ProjectStatCollector() noexcept override;

    public slots:

        void onProjectAboutToChange(ProjectModel* oldModel);

        void onProjectChanged(ProjectModel* model);

    private slots:

        void onTimeOut();

        void onProjectSaved(QDir const& saveDir);

    private:
        void setupWatcher() noexcept;

        internal::AnalysisJob makeJob() const noexcept;

        std::vector<internal::Node> buildNodeTree(QModelIndex const& root) const noexcept;

        void storeResults(StatDataRow result) noexcept;

        ProjectModel* m_model = nullptr;
        QTimer m_timer;
        int m_watchInterval = 30000;//1800000; // 30 minutes interval
        ConnectionWrapper m_timeoutConnection;
        ConnectionWrapper m_projectSavedConnection;
        QFuture<StatDataRow> m_future;
        std::vector<StatDataRow> m_dataPoints;

        inline static std::string const s_filename = "stats.csv";
    };
}

#endif //NOVELIST_PROJECTSTATCOLLECTOR_H
