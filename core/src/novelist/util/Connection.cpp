/**********************************************************
 * @file   Connection.cpp
 * @author jan
 * @date   10/8/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "util/Connection.h"

namespace novelist {

    Connection::Connection() noexcept
            :Connection([]() { return QMetaObject::Connection{}; })
    {
    }

    Connection::Connection(std::function<QMetaObject::Connection()> setupFun, bool autoConnect) noexcept
            :m_setupFun(std::move(setupFun))
    {
        if (autoConnect)
            m_connection = m_setupFun();
    }

    bool Connection::connect() noexcept
    {
        m_connection = m_setupFun();
        return m_connection.isValid();
    }

    bool Connection::disconnect() noexcept
    {
        m_connection.disconnect();
        return !m_connection.isValid();
    }

    bool Connection::isConnected() const noexcept
    {
        return m_connection.isValid();
    }

    ConnectionBlocker::ConnectionBlocker(Connection& connection) noexcept
            :m_connection(connection)
    {
        m_connection.disconnect();
    }

    ConnectionBlocker::~ConnectionBlocker() noexcept
    {
        m_connection.connect();
    }
}