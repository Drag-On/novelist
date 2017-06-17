/**********************************************************
 * @file   Language.cpp
 * @author jan
 * @date   17.06.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <QtCore/QCoreApplication>
#include "lang/Language.h"

namespace novelist {
    Language::Language(QString const& shortname) noexcept
            :m_short(shortname)
    {
    }

    QString const& Language::shortname() const
    {
        return m_short;
    }

    QString Language::name() const
    {
        return qtTrId((m_short + "_long_name").toStdString().c_str());
    }
}