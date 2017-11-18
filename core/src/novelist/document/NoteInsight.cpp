/**********************************************************
 * @file   NoteInsight.cpp
 * @author jan
 * @date   10/28/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "windows/NoteEditWindow.h"
#include "document/NoteInsight.h"

namespace novelist {

    NoteInsight::NoteInsight(gsl::not_null<SceneDocument*> doc, int left, int right, QString msg)
            :BaseInsight(doc, left, right, std::move(msg))
    {
        connect(&m_editAction, &QAction::triggered, this, &NoteInsight::onEditAction);
        connect(&m_removeAction, &QAction::triggered, this, &NoteInsight::onRemoveAction);

        m_menu.addAction(&m_editAction);
        m_menu.addAction(&m_removeAction);
    }

    QTextCharFormat const& NoteInsight::format() const noexcept
    {
        static QTextCharFormat const noteFormat = [] {
            QTextCharFormat format;
            format.setBackground(QColor::fromRgb(180, 255, 145));
            return format;
        }();
        return noteFormat;
    }

    bool NoteInsight::isPersistent() const noexcept
    {
        return true;
    }

    QMenu const& NoteInsight::menu() const noexcept
    {
        return m_menu;
    }

    void NoteInsight::retranslate() noexcept
    {
        setCategory(tr("Note"));

        m_editAction.setText(tr("Edit"));
        m_removeAction.setText(tr("Remove"));

        m_menu.setTitle(tr("Note"));
    }

    void NoteInsight::onEditAction()
    {
        NoteEditWindow wnd;
        wnd.setText(message());
        auto result = wnd.exec();
        if(result == QDialog::Accepted)
            setMessage(wnd.text());
    }

    void NoteInsight::onRemoveAction()
    {
        postRemoveEvent();
    }
}