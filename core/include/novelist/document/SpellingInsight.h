/**********************************************************
 * @file   SpellingInsight.h
 * @author jan
 * @date   10/30/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_SPELLINGINSIGHT_H
#define NOVELIST_SPELLINGINSIGHT_H

#include "AutoInsight.h"

namespace novelist {
    /**
     * A non-persistent insight that is used to represent spelling errors
     */
    class SpellingInsight : public AutoInsight {
    public:
        using AutoInsight::AutoInsight;

        QTextCharFormat const& format() const noexcept override;

        QString const& category() const noexcept override;
    };
}

#endif //NOVELIST_SPELLINGINSIGHT_H
