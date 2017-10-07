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
#include "ConnectionWrapper.h"

namespace novelist {
    /**
     * Action that delegates to another action (or set of member functions). It always delegates to no or one action,
     * and the source can always be switched.
     * @note Currently this will take over the underlying action's triggered() and canTrigger() functionality and take
     *       on its text. Other properties are disregarded.
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
         * @param action New action to delegate to
         */
        void setDelegate(QAction* action) noexcept;

        /**
         * Switch to a set of member functions
         * @tparam T Object type
         * @tparam U Type convertible to T
         * @tparam V Type convertible to T
         * @tparam W Type convertible to T
         * @param src Source object
         * @param trigger Member function to trigger the action
         * @param canTrigger Member function allowing to check if the action can be triggered
         * @param canTriggerChanged Signal that fires when the trigger availablity changes
         * @param text Descriptive text
         */
        template<typename T, typename U, typename V, typename W, typename = std::enable_if<
                std::is_convertible_v<T*, U*> && std::is_convertible_v<T*, V*> && std::is_convertible_v<T*, W*>>>
        void setDelegate(T* src, void (U::* trigger)(), bool (V::* canTrigger)() const,
                void (W::* canTriggerChanged)(), QString const& text = "") noexcept;

        /**
         * Switch to a set of member functions
         * @tparam T Object type
         * @tparam U Type convertible to T
         * @tparam V Type convertible to T
         * @tparam W Type convertible to T
         * @param src Source object
         * @param trigger Member function to trigger the action
         * @param canTrigger Member function allowing to check if the action can be triggered
         * @param canTriggerChanged Signal that fires when the trigger availablity changes
         * @param text Descriptive text
         */
        template<typename T, typename U, typename V, typename W, typename = std::enable_if<
                std::is_convertible_v<T*, U*> && std::is_convertible_v<T*, V*> && std::is_convertible_v<T*, W*>>>
        void setDelegate(T* src, void (U::* trigger)(), bool (V::* canTrigger)() const,
                void (W::* canTriggerChanged)(bool), QString const& text = "") noexcept;

    signals:
        /**
         * Fires whenever the delegate was changed
         */
        void delegateChanged();

    private:
        ConnectionWrapper m_canTriggerChangedConnection;
        ConnectionWrapper m_textChangedConnection;
        std::function<void()> m_triggerFun;
        std::function<bool()> m_canTriggerFun;

    private slots:

        void onCanTriggerChanged();

        void onTriggered(bool checked = false);
    };

    template<typename T, typename U, typename V, typename W, typename = std::enable_if<
            std::is_convertible_v<T*, U*> && std::is_convertible_v<T*, V*> && std::is_convertible_v<T*, W*>>>
    void DelegateAction::setDelegate(T* src, void (U::* trigger)(), bool (V::* canTrigger)() const,
            void (W::* canTriggerChanged)(), QString const& text) noexcept
    {
        if(!text.isEmpty())
            setText(text);

        m_triggerFun = std::bind(trigger, src);
        m_canTriggerFun = std::bind(canTrigger, src);

        onCanTriggerChanged();
        m_canTriggerChangedConnection = connect(src, canTriggerChanged, this, &DelegateAction::onCanTriggerChanged);

        emit delegateChanged();
    }

    template<typename T, typename U, typename V, typename W, typename = std::enable_if<
            std::is_convertible_v<T*, U*> && std::is_convertible_v<T*, V*> && std::is_convertible_v<T*, W*>>>
    void DelegateAction::setDelegate(T* src, void (U::* trigger)(), bool (V::* canTrigger)() const,
            void (W::* canTriggerChanged)(bool), QString const& text) noexcept
    {
        if(!text.isEmpty())
            setText(text);

        m_triggerFun = std::bind(trigger, src);
        m_canTriggerFun = std::bind(canTrigger, src);

        onCanTriggerChanged();
        m_canTriggerChangedConnection = connect(src, canTriggerChanged, [this](bool) { onCanTriggerChanged(); });

        emit delegateChanged();
    }
}

#endif //NOVELIST_UNDOMANAGER_H
