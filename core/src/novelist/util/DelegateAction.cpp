/**********************************************************
 * @file   UndoManager.cpp
 * @author jan
 * @date   10/7/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "util/DelegateAction.h"
#include <gsl/gsl>
#include "util/Connection.h"

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
        setDelegate(nullptr);
    }

    void DelegateAction::setDelegate(QAction* action) noexcept
    {
        m_action = action;

        if (m_action != nullptr) {
            m_delegateChangedConnection = Connection([this] {
                return connect(m_action, &QAction::changed, this, &DelegateAction::onDelegateChanged);
            }, false);
            m_delegateToggledConnection = Connection([this] {
                return connect(m_action, &QAction::toggled, this, &DelegateAction::onDelegateToggled);
            }, false);
            m_delegateTriggeredConnection = Connection([this] {
                return connect(m_action, &QAction::toggled, this, &DelegateAction::onDelegateTriggered);
            }, false);

            if (m_keepSelfUpdated) {
                onDelegateChanged();
                m_delegateChangedConnection.connect();
                m_delegateToggledConnection.connect();
                m_delegateTriggeredConnection.connect();
            }

            m_changedConnection = Connection([this] {
                return connect(this, &DelegateAction::changed, this, &DelegateAction::onChanged);
            }, false);
            m_toggledConnection = Connection([this] {
                return connect(this, &DelegateAction::toggled, this, &DelegateAction::onToggled);
            }, false);
            m_triggeredConnection = Connection([this] {
                return connect(this, &DelegateAction::triggered, this, &DelegateAction::onTriggered);
            }, false);

            if (m_keepDelegateUpdated) {
                m_changedConnection.connect();
                m_toggledConnection.connect();
                m_triggeredConnection.connect();
            }
        }
        else {
            m_delegateChangedConnection = Connection();
            m_delegateToggledConnection = Connection();
            m_delegateTriggeredConnection = Connection();
            m_changedConnection = Connection();
            m_toggledConnection = Connection();
            m_triggeredConnection = Connection();
            setEnabled(false);
        }

        emit delegateReplaced();
    }

    void DelegateAction::setKeepDelegateUpdated(bool updateDelegate) noexcept
    {
        m_keepDelegateUpdated = updateDelegate;
    }

    bool DelegateAction::isKeepDelegateUpdated() const noexcept
    {
        return m_keepDelegateUpdated;
    }

    void DelegateAction::setKeepSelfUpdated(bool updateSelf) noexcept
    {
        m_keepSelfUpdated = updateSelf;
    }

    bool DelegateAction::isKeepSelfUpdated() const noexcept
    {
        return m_keepSelfUpdated;
    }

    void DelegateAction::onDelegateChanged()
    {
        // Block the internal changed-connection because we're changing it ourselves
        ConnectionBlocker blockChangedConnection(m_changedConnection);
        {
            // Also block individual call to changed for property, as this is superfluous
            const QSignalBlocker blocker(this);
            setText(m_action->text());
            setCheckable(m_action->isCheckable());
            setEnabled(m_action->isEnabled());
            setAutoRepeat(m_action->autoRepeat());
            setFont(m_action->font());
            setIcon(m_action->icon());
            setIconText(m_action->iconText());
            setMenuRole(m_action->menuRole());
            setPriority(m_action->priority());
            setShortcut(m_action->shortcut());
            setShortcutContext(m_action->shortcutContext());
            setStatusTip(m_action->statusTip());
            setToolTip(m_action->toolTip());
            setVisible(m_action->isVisible());
            setWhatsThis(m_action->whatsThis());
        }

        // Signal external handlers that this action was changed
        emit changed();
    }

    void DelegateAction::onDelegateToggled(bool checked)
    {
        // Block the internal toggled-connection because we're changing it ourselves
        ConnectionBlocker blockToggledConnection(m_toggledConnection);

        setChecked(checked);
    }

    void DelegateAction::onDelegateTriggered(bool checked)
    {
        if (isCheckable()) {
            ConnectionBlocker blockToggledConnection(m_toggledConnection);
            setChecked(checked);
        }
        else {
            ConnectionBlocker blockTriggeredConnection(m_triggeredConnection);
            trigger();
        }
    }

    void DelegateAction::onChanged()
    {
        Expects(m_action != nullptr);

        // Block the internal changed-connection because we're changing it ourselves
        ConnectionBlocker blockChangedConnection(m_delegateChangedConnection);
        {
            m_action->setText(text());
            m_action->setCheckable(isCheckable());
            m_action->setEnabled(isEnabled());
            m_action->setAutoRepeat(autoRepeat());
            m_action->setFont(font());
            m_action->setIcon(icon());
            m_action->setIconText(iconText());
            m_action->setMenuRole(menuRole());
            m_action->setPriority(priority());
            m_action->setShortcut(shortcut());
            m_action->setShortcutContext(shortcutContext());
            m_action->setStatusTip(statusTip());
            m_action->setToolTip(toolTip());
            m_action->setVisible(isVisible());
            m_action->setWhatsThis(whatsThis());
        }
    }

    void DelegateAction::onToggled(bool checked)
    {
        Expects(m_action != nullptr);

        ConnectionBlocker blockToggledConnection(m_delegateToggledConnection);
        m_action->setChecked(checked);
    }

    void DelegateAction::onTriggered(bool checked)
    {
        Expects(m_action != nullptr);

        if (m_action->isCheckable()) {
            ConnectionBlocker blockToggledConnection(m_delegateToggledConnection);
            m_action->setChecked(checked);
        }
        else {
            ConnectionBlocker blockTriggeredConnection(m_delegateTriggeredConnection);
            m_action->trigger();
        }
    }
}