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
#include "widgets/texteditor/TextEditorInsightManager.h"
#include "widgets/texteditor/TextEditor.h"

namespace novelist {
    TextEditorInsightManager::TextEditorInsightManager(gsl::not_null<TextEditor*> editor) noexcept
            :QObject(nullptr),
             m_editor(editor)
    {
    }

    void TextEditorInsightManager::onMousePosChanged(QPoint pos)
    {
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
}