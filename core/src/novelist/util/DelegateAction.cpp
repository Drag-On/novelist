/**********************************************************
 * @file   UndoManager.cpp
 * @author jan
 * @date   10/7/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "util/DelegateAction.h"

namespace novelist {
    DelegateAction::DelegateAction(QObject* parent) noexcept
            :DelegateAction("", parent)
    {
    }

    DelegateAction::DelegateAction(QString const& text, QObject* parent) noexcept
            :DelegateAction(QIcon{}, text, parent)
    {
    }

    DelegateAction::DelegateAction(QIcon const& icon, QString const& text, QObject* parent) noexcept
            :QAction(icon, text, parent)
    {
        connect(this, &DelegateAction::triggered, this, &DelegateAction::onTriggered);
        setEnabled(false);
    }

    void DelegateAction::setDelegate(QAction* action) noexcept
    {
        setText(action->text());

        m_triggerFun = std::bind(&QAction::trigger, action);
        m_canTriggerFun = std::bind(&QAction::isEnabled, action);

        onCanTriggerChanged();
        m_canTriggerChangedConnection = connect(action, &QAction::changed, this, &DelegateAction::onCanTriggerChanged);
        m_textChangedConnection = connect(action, &QAction::changed, [this, action](){ setText(action->text()); });

        emit delegateChanged();
    }

    void DelegateAction::onCanTriggerChanged()
    {
        bool canTrigger = m_canTriggerFun();
        if (canTrigger != isEnabled()) {
            setEnabled(canTrigger);
            emit changed();
        }
    }

    void DelegateAction::onTriggered(bool /*checked*/)
    {
        m_triggerFun();
    }
}