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
#include "widgets/texteditor/TextAnnotation.h"
#include "widgets/texteditor/InsightManager.h"

namespace novelist {
    InsightManager::InsightManager(gsl::not_null<TextEditor*> editor) noexcept
            :m_editor(editor)
    {
    }

    void InsightManager::onMousePosChanged(QPoint pos)
    {
        auto const& insights = m_editor->m_insights;
        for (int row = 0; row < insights.rowCount(); ++row) {
            IInsight* insight = qvariant_cast<IInsight*>(
                    insights.data(insights.index(row, 0), static_cast<int>(InsightModelRoles::DataRole)));

            auto cursor = insight->toCursor();
            QTextCursor leftCursor(cursor.document());
            leftCursor.setPosition(cursor.selectionStart());
            QTextCursor rightCursor(cursor.document());
            rightCursor.setPosition(cursor.selectionEnd());
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