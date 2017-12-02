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
    class NOVELIST_CORE_EXPORT SpellingInsight : public AutoInsight {
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

#endif //NOVELIST_SPELLINGINSIGHT_H
