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
}