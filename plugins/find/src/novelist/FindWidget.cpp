/**********************************************************
 * @file   FindWidget.cpp
 * @author jan
 * @date   12/17/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtCore/QEvent>
#include <QtWidgets/QProgressDialog>
#include <QtCore/QTime>
#include <QtGui/QStandardItemModel>
#include <windows/MainWindow.h>
#include "FindWidget.h"
#include "ui_FindWidget.h"

namespace novelist {
    FindWidget::FindWidget(QWidget* parent) noexcept
            :QWidget(parent),
             m_ui(std::make_unique<Ui::FindWidget>())
    {
        m_ui->setupUi(this);
        setupConnections();
    }

    void FindWidget::changeEvent(QEvent* event)
    {
        QWidget::changeEvent(event);
        switch (event->type()) {
            case QEvent::LanguageChange:
                m_ui->retranslateUi(this);
                break;
            default:
                break;
        }
    }

    void FindWidget::setupConnections() noexcept
    {
        connect(m_ui->pushButtonSearch, &QPushButton::pressed, this, &FindWidget::onSearchStarted);
    }

    std::pair<ProjectModel*, QModelIndex> FindWidget::getSearchModelRoot() noexcept
    {
        std::pair<ProjectModel*, QModelIndex> result;

        MainWindow* mainWin = nullptr;
        for (QWidget* w : qApp->topLevelWidgets()) {
            auto* window = dynamic_cast<MainWindow*>(w);
            if (window) {
                mainWin = window;
                break;
            }
        }

        if (mainWin == nullptr)
            return result;

        result.first = mainWin->project();

        switch (m_ui->comboBoxScope->currentIndex()) {
            case 0: // Scene
            {
                auto [m, idx] = mainWin->sceneTabWidget()->current();
                if (m == result.first) {
                    result.second = idx;
                    return result;
                }
                break;
            }
            case 1: // Chapter
            {
                auto [m, idx] = mainWin->sceneTabWidget()->current();
                if (m == result.first) {
                    result.second = idx.parent();
                    return result;
                }
                break;
            }
            case 2: // Project
            {
                result.second = result.first->projectRootIndex();
                break;
            }
            default:
                break;
        }

        return result;
    }

    void FindWidget::search(ProjectModel* model, QModelIndex root, QAbstractItemModel& resultsModel) noexcept
    {
        // TODO: This is a placeholder
        QTime dieTime= QTime::currentTime().addSecs(1);
        while (QTime::currentTime() < dieTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    void FindWidget::onSearchStarted()
    {
        QString searchTerm = m_ui->lineEditFind->text();
        auto [model, idx] = getSearchModelRoot();

        if (model == nullptr || !idx.isValid())
            return;

        int maxNum = 1;
        if (m_ui->comboBoxScope->currentIndex() > 0)
            maxNum = model->rowCount(idx);

        QStandardItemModel resultModel;

        QProgressDialog progress(tr("Looking for matches..."), tr("Abort"), 0, maxNum, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.setMinimumDuration(1000); // Don't show dialog if finished in less than 1 second

        for (int i = 0; i < maxNum; i++) {
            progress.setValue(i);

            if (progress.wasCanceled())
                break;

            search(model, idx, resultModel);
        }
        progress.setValue(maxNum);
    }
}