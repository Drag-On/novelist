/**********************************************************
 * @file   SpellingInsight.cpp
 * @author jan
 * @date   10/30/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "document/SpellingInsight.h"

namespace novelist {

    QTextCharFormat const& SpellingInsight::format() const noexcept
    {
        static QTextCharFormat const spellingFormat = [] {
            QTextCharFormat format;
            format.setUnderlineStyle(QTextCharFormat::WaveUnderline);
            format.setUnderlineColor(QColor::fromRgb(255, 0, 0));
            return format;
        }();
        return spellingFormat;
    }

    void SpellingInsight::retranslate() noexcept
    {
        AutoInsight::retranslate();
        setCategory(tr("Spelling"));
    }
}