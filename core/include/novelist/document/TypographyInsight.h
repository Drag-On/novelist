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
    Q_OBJECT
    
    public:
        QTextCharFormat const& format() const noexcept override;

        void retranslate() noexcept override;

    protected:
        using AutoInsight::AutoInsight;

        template <typename, typename>
        friend class BaseInsightFactory;
    };
}

#endif //NOVELIST_TYPOGRAPHYINSIGHT_H
