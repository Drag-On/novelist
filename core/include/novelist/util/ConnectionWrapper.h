/**********************************************************
 * @file   ConenctionWrapper.h
 * @author jan
 * @date   10/2/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_CONENCTIONWRAPPER_H
#define NOVELIST_CONENCTIONWRAPPER_H

#include <qobjectdefs.h>

namespace novelist {
    /**
     * Wraps a Qt signal-slot connection and automatically disconnects it on destruction.
     *
     * @example ConnectionWrapper con = QObject::connect(obj, &QObject::destroyed, otherObj, &MyObject::onDestroyed);
     */
    class ConnectionWrapper {
    public:
        ConnectionWrapper() = default;

        ConnectionWrapper(QMetaObject::Connection connection) noexcept;

        ConnectionWrapper(QMetaObject::Connection&& connection) noexcept;

        ~ConnectionWrapper() noexcept;

        ConnectionWrapper& operator=(QMetaObject::Connection const& connection) noexcept;

        ConnectionWrapper& operator=(QMetaObject::Connection&& connection) noexcept;

    private:
        QMetaObject::Connection m_connection;
    };
}

#endif //NOVELIST_CONENCTIONWRAPPER_H
