/**********************************************************
 * @file   AutoInsight.h
 * @author jan
 * @date   10/30/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_AUTOINSIGHT_H
#define NOVELIST_AUTOINSIGHT_H

#include "BaseInsight.h"

namespace novelist {
    /**
     * Common base class for insights that are not user-generated, but rather by tools such as spellcheckers
     */
    class AutoInsight : public BaseInsight {
    public:
        using BaseInsight::BaseInsight;

        bool isPersistent() const noexcept override;

        QMenu const& menu() const noexcept override;

    private:
        QMenu m_menu;
    };
}

#endif //NOVELIST_AUTOINSIGHT_H
