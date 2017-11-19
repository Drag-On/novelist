/**********************************************************
 * @file   ProjectStatCollector.cpp
 * @author jan
 * @date   11/19/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "ProjectStatCollector.h"
#include <QtConcurrent/QtConcurrent>
#include <util/Overloaded.h>

namespace novelist {
    namespace internal {
        void analyzeNode(AnalysisResult& result, Node const& n) {
            result.m_numCharacters += n.m_name.size() + n.m_text.size();
            for (auto const& c : n.m_children)
                analyzeNode(result, c);
        }

        AnalysisResult analyze(AnalysisJob job) noexcept
        {
            AnalysisResult result{};
            result.m_timeStamp = job.m_timeStamp;

            for (auto const& n : job.m_nodes)
                analyzeNode(result, n);

            return result;
        }
    }

    ProjectStatCollector::~ProjectStatCollector() noexcept
    {
        m_timer.stop();
        if (m_future.isRunning())
            m_future.waitForFinished();
    }

    void ProjectStatCollector::onProjectAboutToChange(ProjectModel* oldModel)
    {
        qDebug() << "onProjectAboutToChange" << oldModel;
        m_timer.stop();
    }

    void ProjectStatCollector::onProjectChanged(ProjectModel* model)
    {
        qDebug() << "onProjectChanged" << model;
        m_model = model;
        if(m_model)
            setupWatcher();
        else
            m_timer.stop();
    }

    void ProjectStatCollector::onTimeOut()
    {
        qDebug() << "onTimeOut";
        Expects(m_model != nullptr);

        if (m_future.isFinished()) {
            if (!m_future.isCanceled())
                storeResults(std::move(m_future.result()));
            auto job = makeJob();
            m_future = QtConcurrent::run(internal::analyze, job);
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

        for (int i = 0; i < m_model->rowCount(root); ++i)
        {
            QModelIndex const& child = root.child(i, 0);
            auto const& childData = m_model->nodeData(child);
            internal::Node n = std::visit(makeNodeFun, *childData);
            n.m_children = buildNodeTree(child);
            nodes.emplace_back(n);
        }

        return nodes;
    }

    void ProjectStatCollector::storeResults(internal::AnalysisResult result) noexcept
    {
        qDebug() << result.m_timeStamp << result.m_numCharacters;
        m_dataPoints.emplace_back(std::move(result));
    }
}