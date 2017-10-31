/**********************************************************
 * @file   TypographyInsight.h
 * @author jan
 * @date   10/31/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TYPOGRAPHYINSIGHT_H
#define NOVELIST_TYPOGRAPHYINSIGHT_H

#include "AutoInsight.h"

namespace novelist {
    class TypographyInsight : public AutoInsight {
    public:
        using AutoInsight::AutoInsight;

        QTextCharFormat const& format() const noexcept override;

        QString const& category() const noexcept override;
    };
}

#endif //NOVELIST_TYPOGRAPHYINSIGHT_H
