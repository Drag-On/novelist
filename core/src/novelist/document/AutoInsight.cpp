/**********************************************************
 * @file   AutoInsight.cpp
 * @author jan
 * @date   10/30/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "document/AutoInsight.h"

namespace novelist {
    AutoInsight::AutoInsight(gsl::not_null<SceneDocument*> doc, int left, int right, QString msg,
            QStringList suggestions)
            :BaseInsight(doc, left, right, msg),
             m_suggestions(std::move(suggestions))
    {
        m_menu.setTitle(tr("Suggestions"));
        for (QString const& s : m_suggestions) {
            auto* act = new QAction(s, &m_menu);
            connect(act, &QAction::triggered, [this, s] {
                replaceMarkedText(s);
            });
            m_menu.addAction(act);
        }
    }

    bool AutoInsight::isPersistent() const noexcept
    {
        return false;
    }

    QMenu const& AutoInsight::menu() const noexcept
    {
        return m_menu;
    }
}