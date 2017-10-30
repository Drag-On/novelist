/**********************************************************
 * @file   GrammarInsight.cpp
 * @author jan
 * @date   10/30/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "document/GrammarInsight.h"

namespace novelist {

    QTextCharFormat const& GrammarInsight::format() const noexcept
    {
        static QTextCharFormat const grammarFormat = [] {
            QTextCharFormat format;
            format.setUnderlineStyle(QTextCharFormat::WaveUnderline);
            format.setUnderlineColor(QColor::fromRgb(0, 0, 255));
            return format;
        }();
        return grammarFormat;
    }

    QString const& GrammarInsight::category() const noexcept
    {
        static QString category = tr("Grammar");
        return category;
    }
}