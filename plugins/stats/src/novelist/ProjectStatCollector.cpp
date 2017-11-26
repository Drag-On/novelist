/**********************************************************
 * @file   ProjectStatCollector.cpp
 * @author jan
 * @date   11/19/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "ProjectStatCollector.h"
#include <fstream>
#include <QtConcurrent/QtConcurrent>
#include <util/Overloaded.h>

namespace novelist {
    namespace internal {
        void analyzeNode(StatDataRow& result, Node const& n)
        {
            result.m_stats = TextAnalyzer::combine(std::move(result.m_stats), TextAnalyzer::analyze(n.m_name),
                    TextAnalyzer::analyze(n.m_text));
            for (auto const& c : n.m_children)
                analyzeNode(result, c);
        }

        StatDataRow analyze(AnalysisJob job) noexcept
        {
            StatDataRow result{};
            result.m_timeStamp = job.m_timeStamp;

            for (auto const& n : job.m_nodes)
                analyzeNode(result, n);

            return result;
        }

        bool onlyTimestampDiffers(StatDataRow const& r1, StatDataRow const& r2) noexcept
        {
            return r1.m_stats == r2.m_stats;
        }
    }

    ProjectStatCollector::ProjectStatCollector() noexcept
    {
        connect(&m_futureWatcher, &QFutureWatcher<StatDataRow>::finished, this, &ProjectStatCollector::onFutureReady);
    }

    ProjectStatCollector::~ProjectStatCollector() noexcept
    {
        m_timer.stop();
        if (m_future.isRunning())
            m_future.waitForFinished();
    }

    void ProjectStatCollector::setWatchInterval(int minutes) noexcept
    {
        m_watchInterval = minutes * 60 * 1000;
    }

    int ProjectStatCollector::getWatchInterval() const noexcept
    {
        return m_watchInterval / 60000;
    }

    char const* ProjectStatCollector::filename() noexcept
    {
        return s_filename;
    }

    void ProjectStatCollector::onProjectAboutToChange(ProjectModel* /*oldModel*/)
    {
        m_timer.stop();
    }

    void ProjectStatCollector::onProjectChanged(ProjectModel* model)
    {
        m_model = model;
        if (m_model) {
            m_projectSavedConnection = connect(m_model, &ProjectModel::projectSaved, this,
                    &ProjectStatCollector::onProjectSaved);
            onTimeOut();
            setupWatcher();
        }
        else {
            m_timer.stop();
            m_projectSavedConnection.disconnect();
        }
    }

    void ProjectStatCollector::onTimeOut()
    {
        Expects(m_model != nullptr);

        if (m_future.isFinished()) {
            auto job = makeJob();
            m_future = QtConcurrent::run(internal::analyze, job);
            m_futureWatcher.setFuture(m_future);
        }
    }

    void ProjectStatCollector::onFutureReady()
    {
        if (m_future.isFinished() && !m_future.isCanceled())
            storeResults(std::move(m_future.result()));
    }

    void ProjectStatCollector::onProjectSaved(QDir const& saveDir)
    {
        if (!m_dataPoints.empty()) {
            std::string filename = saveDir.path().toStdString() + QDir::separator().toLatin1() + s_filename;
            std::ofstream out(filename,
                    std::ofstream::out | std::ofstream::app);
            if (out.is_open()) {
                for (auto const& row : m_dataPoints) {
                    out << row.m_timeStamp.toSecsSinceEpoch() << ","
                        << row.m_stats.m_charCount << ","
                        << row.m_stats.m_wordCount << "\n";
                }
                m_dataPoints.clear();
            }
            else
                qWarning() << "Unable to open" << filename.c_str() << "for writing. Can't write statistics.";
        }
    }

    void ProjectStatCollector::setupWatcher() noexcept
    {
        Expects(m_model != nullptr);

        m_timer.setTimerType(Qt::TimerType::VeryCoarseTimer);
        m_timer.setInterval(m_watchInterval);
        m_timeoutConnection = connect(&m_timer, &QTimer::timeout, this, &ProjectStatCollector::onTimeOut);
        m_timer.start();
    }

    internal::AnalysisJob ProjectStatCollector::makeJob() const noexcept
    {
        internal::AnalysisJob job;
        job.m_timeStamp = QDateTime::currentDateTime();
        job.m_nodes = buildNodeTree(m_model->projectRootIndex());

        return job;
    }

    std::vector<internal::Node> ProjectStatCollector::buildNodeTree(QModelIndex const& root) const noexcept
    {
        std::vector<internal::Node> nodes;

        auto makeNodeFun = Overloaded {
                [](auto const&) {
                    qWarning() << "Unknown model data type.";
                    return internal::Node{};
                },
                [](ProjectModel::SceneData const& arg) {
                    internal::Node node;
                    node.m_name = arg.m_name;
                    node.m_text = arg.m_doc->toRawText();
                    return node;
                },
                [](ProjectModel::ChapterData const& arg) {
                    internal::Node node;
                    node.m_name = arg.m_name;
                    return node;
                },
        };

        for (int i = 0; i < m_model->rowCount(root); ++i) {
            QModelIndex const& child = root.child(i, 0);
            auto const& childData = m_model->nodeData(child);
            internal::Node n = std::visit(makeNodeFun, *childData);
            n.m_children = buildNodeTree(child);
            nodes.emplace_back(n);
        }

        return nodes;
    }

    void ProjectStatCollector::storeResults(StatDataRow result) noexcept
    {
        if (!m_dataPoints.empty()) {
            auto& lastRow = m_dataPoints.back();
            if (internal::onlyTimestampDiffers(lastRow, result)) {
                lastRow.m_timeStamp = result.m_timeStamp;
                return;
            }
        }
        m_dataPoints.emplace_back(std::move(result));
    }
}