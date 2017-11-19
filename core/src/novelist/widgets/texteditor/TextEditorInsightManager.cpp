/**********************************************************
 * @file   InsightManager.cpp
 * @author jan
 * @date   10/23/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtWidgets/QToolTip>
#include <QtGui/QTextCursor>
#include <QtCore/QCoreApplication>
#include <QtConcurrent/QtConcurrent>
#include "widgets/texteditor/TextEditorInsightManager.h"
#include "widgets/texteditor/Inspector.h"
#include "widgets/texteditor/TextEditor.h"

namespace novelist {

    TextEditorInsightManager::TextEditorInsightManager(gsl::not_null<TextEditor*> editor) noexcept
            :QObject(nullptr),
             m_editor(editor)
    {
        onDocumentChanged();
        connect(&m_updateTimer, &QTimer::timeout, this, &TextEditorInsightManager::onUpdate);
        m_updateTimer.start(1000);
    }

    TextEditorInsightManager::~TextEditorInsightManager() noexcept
    {
        m_updateTimer.stop();
        if (m_updateResults.isRunning())
            m_updateResults.waitForFinished();
    }

    void TextEditorInsightManager::reinspect() noexcept
    {
        if (m_editor->document()) {
            m_needUpdateBlocks.clear();
            for (auto b = m_editor->document()->begin(); b != m_editor->document()->end(); b = b.next())
                m_needUpdateBlocks.push_back(b);
        }
    }

    void TextEditorInsightManager::onMousePosChanged(QPoint pos)
    {
        // Show tool tip on hover
        auto const& insights = m_editor->m_insights;
        for (int row = 0; row < insights.rowCount(); ++row) {
            auto* insight = qvariant_cast<Insight*>(
                    insights.data(insights.index(row, 0), static_cast<int>(InsightModelRoles::InsightDataRole)));

            auto range = insight->range();
            QTextCursor leftCursor(insight->document());
            leftCursor.setPosition(range.first);
            QTextCursor rightCursor(insight->document());
            rightCursor.setPosition(range.second);
            QRect cursorRect = m_editor->cursorRect(leftCursor);
            QRect otherRect = m_editor->cursorRect(rightCursor);
            QRect selectionRect = cursorRect.united(otherRect);
            if (selectionRect.height() > cursorRect.height()) {
                selectionRect.setLeft(0);
                selectionRect.setRight(m_editor->size().width());
            }
            if (selectionRect.contains(pos))
                QToolTip::showText(m_editor->mapToGlobal(pos), insight->message(), m_editor, selectionRect);
        }
    }

    void TextEditorInsightManager::onDocumentChanged()
    {
        if (m_editor->document()) {
            m_contentsChangeConnection = connect(m_editor->document(),
                    &SceneDocument::contentsChange, this, &TextEditorInsightManager::onContentsChange);
            reinspect();
        }
        else
            m_contentsChangeConnection.disconnect();
    }

    void TextEditorInsightManager::startAutoInsightRefresh()
    {
        std::vector<QString> blocks;
        for (auto const& e : m_needUpdateBlocks)
            blocks.push_back(e.text());

        m_updatingBlocks = std::move(m_needUpdateBlocks);
        m_updateResults = QtConcurrent::run(runAutoInsightRefresh, blocks, m_editor->m_inspectors,
                m_editor->document()->language());
    }

    void TextEditorInsightManager::finishAutoInsightRefresh()
    {
        if (m_editor->document() == nullptr)
            return;

        auto const& blocks = m_updateResults.result();
        Q_ASSERT(blocks.size() == m_updatingBlocks.size());
        for (size_t i = 0; i < blocks.size(); ++i) {
            auto const& b = blocks[i];
            auto const& ub = m_updatingBlocks[i];
            if (!ub.isValid())
                continue;

            m_editor->m_insights.removeNonPersistentInRange(ub.position(), ub.position() + ub.length());

            for (auto const& insight : b) {
                int left = ub.position() + insight.m_left;
                int right = ub.position() + insight.m_right;
                auto ptr = insight.m_factory->create(m_editor->document(), left, right);
                if (ptr)
                    m_editor->m_insights.insert(std::move(ptr));
            }
        }
        m_updateResults = decltype(m_updateResults)();
    }

    InspectionResult TextEditorInsightManager::runAutoInsightRefresh(std::vector<QString> blocks,
            std::vector<std::unique_ptr<Inspector>> const* inspectors, Language lang)
    {
        InspectionResult results;
        for (auto const& text : blocks) {

            InspectionBlockResult blockResult{};
            for (auto const& inspector : *inspectors) {
                auto result = inspector->inspect(text, lang);
                blockResult.insert(blockResult.end(), result.begin(), result.end());
            }

            results.push_back(std::move(blockResult));
        }

        return results;
    }

    void TextEditorInsightManager::onContentsChange(int pos, int /*removed*/, int /*added*/)
    {
        // Mark the changed block for future refresh of auto insights
        auto block = m_editor->document()->findBlock(pos);
        if (std::count(m_needUpdateBlocks.begin(), m_needUpdateBlocks.end(), block) == 0)
            m_needUpdateBlocks.push_back(block);
    }

    void TextEditorInsightManager::onUpdate()
    {
        if (m_updateResults.isFinished()) {
            if (!m_updateResults.isCanceled()) // Future is set as finished and cancelled if default-constructed
                finishAutoInsightRefresh();
            if (!m_needUpdateBlocks.empty())
                startAutoInsightRefresh();
        }
    }
}