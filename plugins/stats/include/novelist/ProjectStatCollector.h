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

namespace novelist {
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

        struct AnalysisResult {
            QDateTime m_timeStamp;
            size_t m_numCharacters;
        };

        AnalysisResult analyze(AnalysisJob job) noexcept;
    }

    class ProjectStatCollector : public QObject {
    Q_OBJECT

    public:
        ~ProjectStatCollector() noexcept;

    public slots:

        void onProjectAboutToChange(ProjectModel* oldModel);

        void onProjectChanged(ProjectModel* model);

    private slots:

        void onTimeOut();

    private:
        void setupWatcher() noexcept;

        internal::AnalysisJob makeJob() const noexcept;

        std::vector<internal::Node> buildNodeTree(QModelIndex const& root) const noexcept;

        void storeResults(internal::AnalysisResult result) noexcept;

        ProjectModel* m_model = nullptr;
        QTimer m_timer;
        int m_watchInterval = 30000;//1800000; // 30 minutes interval
        ConnectionWrapper m_timeoutConnection;
        QFuture<internal::AnalysisResult> m_future;
        std::vector<internal::AnalysisResult> m_dataPoints;
    };
}

#endif //NOVELIST_PROJECTSTATCOLLECTOR_H
