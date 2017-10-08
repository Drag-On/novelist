/**********************************************************
 * @file   ConnectionWrapper.cpp
 * @author jan
 * @date   10/2/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <QtCore/QObject>
#include "util/ConnectionWrapper.h"

namespace novelist {
    ConnectionWrapper::ConnectionWrapper(ConnectionWrapper const& other) noexcept
        : m_connection(other.m_connection)
    {
    }

    ConnectionWrapper::ConnectionWrapper(ConnectionWrapper&& other) noexcept
        : m_connection(std::move(other.m_connection))
    {
    }

    ConnectionWrapper::ConnectionWrapper(QMetaObject::Connection connection) noexcept
            :m_connection(std::move(connection))
    {
    }

    ConnectionWrapper::ConnectionWrapper(QMetaObject::Connection&& connection) noexcept
            :m_connection(connection)
    {
    }

    ConnectionWrapper::~ConnectionWrapper() noexcept
    {
        if (m_connection)
            QObject::disconnect(m_connection);
    }

    ConnectionWrapper& ConnectionWrapper::operator=(QMetaObject::Connection const& connection) noexcept
    {
        if(&m_connection != &connection) {
            if (m_connection)
                QObject::disconnect(m_connection);
            m_connection = connection;
        }

        return *this;
    }

    ConnectionWrapper& ConnectionWrapper::operator=(QMetaObject::Connection&& connection) noexcept
    {
        if(&m_connection != &connection) {
            if (m_connection)
                QObject::disconnect(m_connection);
            m_connection = connection;
        }

        return *this;
    }

    ConnectionWrapper& ConnectionWrapper::operator=(ConnectionWrapper const& other) noexcept
    {
        if(&m_connection != &other.m_connection) {
            if (m_connection)
                QObject::disconnect(m_connection);
            m_connection = other.m_connection;
        }

        return *this;
    }

    ConnectionWrapper& ConnectionWrapper::operator=(ConnectionWrapper&& other) noexcept
    {
        if(&m_connection != &other.m_connection) {
            if (m_connection)
                QObject::disconnect(m_connection);
            m_connection = std::move(other.m_connection);
        }

        return *this;
    }

    bool ConnectionWrapper::disconnect() noexcept
    {
        return QObject::disconnect(m_connection);
    }

    bool ConnectionWrapper::isValid() const noexcept
    {
        return static_cast<bool>(m_connection);
    }
}