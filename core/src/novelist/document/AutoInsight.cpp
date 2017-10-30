/**********************************************************
 * @file   AutoInsight.cpp
 * @author jan
 * @date   10/30/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "document/AutoInsight.h"

namespace novelist {

    bool AutoInsight::isPersistent() const noexcept
    {
        return false;
    }

    QMenu const& AutoInsight::menu() const noexcept
    {
        return m_menu;
    }
}