/**********************************************************
 * @file   UndoManager.h
 * @author jan
 * @date   10/7/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_UNDOMANAGER_H
#define NOVELIST_UNDOMANAGER_H

#include <QtWidgets/QAction>
#include <QDebug>
#include "Connection.h"

namespace novelist {
    /**
     * Action that delegates to another action. It always delegates to no or one action, and the source can be exchanged
     * at any point.
     */
    class DelegateAction : public QAction {
    Q_OBJECT

    public:
        /**
         * Constructor
         * @param parent Parent object
         */
        explicit DelegateAction(QObject* parent = nullptr) noexcept;

        /**
         * Constructor
         * @param text Descriptive text
         * @param parent Parent object
         */
        explicit DelegateAction(QString const& text, QObject* parent = nullptr) noexcept;

        /**
         * Constructor
         * @param icon Descriptive icon
         * @param text Descriptive text
         * @param parent Parent object
         */
        DelegateAction(QIcon const& icon, QString const& text, QObject* parent = nullptr) noexcept;

        /**
         * Switch to another action
         * @param action New action to delegate to. If this is nullptr, then the previous delegate is removed and this
         *               action disabled.
         */
        void setDelegate(QAction* action) noexcept;

        /**
         * @param updateDelegate Indicates whether the delegate should be kept up-to-date with modifications of this action
         */
        void setKeepDelegateUpdated(bool updateDelegate) noexcept;

        /**
         * Indicates whether the set delegate is kept up-to-date with this action.
         * @details If this is set to true, then all changes made to this action will be forwarded to the delegate, e.g.
         *          text or icon changes and triggers.
         * @return True if the delegate action is kept up-to-date with changes to this action, otherwise false
         */
        bool isKeepDelegateUpdated() const noexcept;

        /**
         * @param updateSelf Indicates whether this action should be kept up-to-date with modifications to the delegate
         */
        void setKeepSelfUpdated(bool updateSelf) noexcept;

        /**
         * Indicates whether changes of the delegate are also applied to this action
         * @details If this is set to true, then all changes made to the delegate action will be forwarded to this action,
         *          e.g. text or icon changes and triggers.
         * @return True if this action is kept up-to-date with changes to the delegate action, otherwise false
         */
        bool isKeepSelfUpdated() const noexcept;

    signals:
        /**
         * Fires whenever the action delegated to was replaced
         */
        void delegateReplaced();

    private:
        Connection m_delegateChangedConnection;
        Connection m_delegateToggledConnection;
        Connection m_delegateTriggeredConnection;
        Connection m_changedConnection;
        Connection m_toggledConnection;
        Connection m_triggeredConnection;
        QAction* m_action;
        bool m_keepDelegateUpdated = true;
        bool m_keepSelfUpdated = true;

    private slots:

        void onDelegateChanged();

        void onDelegateToggled(bool checked);

        void onDelegateTriggered(bool checked = false);

        void onChanged();

        void onToggled(bool checked);

        void onTriggered(bool checked = false);
    };
}

#endif //NOVELIST_UNDOMANAGER_H
