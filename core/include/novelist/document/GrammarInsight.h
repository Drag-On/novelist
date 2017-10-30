/**********************************************************
 * @file   GrammarInsight.h
 * @author jan
 * @date   10/30/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_GRAMMARINSIGHT_H
#define NOVELIST_GRAMMARINSIGHT_H

#include "AutoInsight.h"

namespace novelist {
    /**
     * A non-persistent insight that is used to represent grammar issues
     */
    class GrammarInsight : public AutoInsight {
    public:
        using AutoInsight::AutoInsight;

        QTextCharFormat const& format() const noexcept override;

        QString const& category() const noexcept override;
    };
}

#endif //NOVELIST_GRAMMARINSIGHT_H
