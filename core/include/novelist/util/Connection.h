/**********************************************************
 * @file   Connection.h
 * @author jan
 * @date   10/8/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_CONNECTION_H
#define NOVELIST_CONNECTION_H

#include <type_traits>
#include <functional>
#include <qobjectdefs.h>
#include "ConnectionWrapper.h"
#include <novelist_core_export.h>

namespace novelist {
    /**
     * Wrapper to a signal-slot connection which can be used to connect and disconnect the same connection at any time
     */
    class NOVELIST_CORE_EXPORT Connection {
    public:
        /**
         * Construct invalid connection
         */
        Connection() noexcept;

        /**
         * Construct a connection using a function object that establishes it
         * @details \p setupFun is called immediately on construction to establish the connection.
         * @param setupFun Function object that established the connection
         * @param autoConnect Determines whether to connect right away
         */
        explicit Connection(std::function<QMetaObject::Connection()> setupFun, bool autoConnect = true) noexcept;

        /**
         * Sets up the connection
         * @return True if the connection is established after the call, either because the connection was successful
         *         or because it was already connected. False otherwise.
         */
        bool connect() noexcept;

        /**
         * Disconnects
         * @return True if the connection is disconnected after the call, either because the disconnection was
         *         successful or because it was already disconnected. False otherwise.
         */
        bool disconnect() noexcept;

        /**
         * @return True if connection is established, otherwise false
         */
        bool isConnected() const noexcept;

    private:
        std::function<QMetaObject::Connection()> m_setupFun;
        ConnectionWrapper m_connection;
    };

    /**
     * Temporarily disconnects a connection. Reconnection happens on destruction.
     */
    class NOVELIST_CORE_EXPORT ConnectionBlocker {
    private:
        Connection& m_connection;

    public:
        explicit ConnectionBlocker(Connection& connection) noexcept;

        ~ConnectionBlocker() noexcept;

        ConnectionBlocker(ConnectionBlocker const&) = delete;

        ConnectionBlocker(ConnectionBlocker&&) = delete;

        ConnectionBlocker& operator=(ConnectionBlocker const&) = delete;

        ConnectionBlocker& operator=(ConnectionBlocker&&) = delete;
    };

}

#endif //NOVELIST_CONNECTION_H
