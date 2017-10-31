/**********************************************************
 * @file   TypographyInsight.cpp
 * @author jan
 * @date   10/31/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtGui/QTextCharFormat>
#include "document/TypographyInsight.h"

namespace novelist {
    QTextCharFormat const& TypographyInsight::format() const noexcept
    {
        static QTextCharFormat const typographyFormat = [] {
            QTextCharFormat format;
            format.setUnderlineStyle(QTextCharFormat::WaveUnderline);
            format.setUnderlineColor(QColor::fromRgb(130, 0, 180));
            return format;
        }();
        return typographyFormat;
    }

    QString const& TypographyInsight::category() const noexcept
    {
        static QString category = tr("Typography");
        return category;
    }
}