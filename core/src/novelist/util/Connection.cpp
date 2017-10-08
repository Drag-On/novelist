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
            :m_setupFun([]() { return QMetaObject::Connection{}; })
    {
        m_connection = m_setupFun();
    }

    Connection::Connection(std::function<QMetaObject::Connection()> setupFun) noexcept
            :m_setupFun(std::move(setupFun))
    {
        auto con = m_setupFun();
        m_connection = con;
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