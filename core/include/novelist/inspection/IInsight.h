/**********************************************************
 * @file   IInspection.h
 * @author jan
 * @date   10/18/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_IINSPECTION_H
#define NOVELIST_IINSPECTION_H

#include <QtCore/QString>
#include <QtWidgets/QMenu>
#include <QTextCursor>

namespace novelist {
    /**
     * Available annotation types
     */
    enum class InsightType {
        Note, //!< Note style, will be displayed with a bright green background
        Info, //!< Info style, will be displayed with a bright violet background
        SpellingError, //!< Spelling error style, will be displayed with red squiggly underline
        GrammarError, //!< Grammar error style, will be displayed with blue squiggly underline
    };

    inline QString insightTypeToDescription(InsightType type)
    {
        switch (type) {
            case InsightType::Info:
                return QObject::tr("Info");
            case InsightType::Note:
                return QObject::tr("Note");
            case InsightType::SpellingError:
                return QObject::tr("Spelling");
            case InsightType::GrammarError:
                return QObject::tr("Grammar");
        }
        return "";
    }

    inline QTextCharFormat const& insightTypeToFormat(InsightType type)
    {
        static QTextCharFormat const s_defaultNoteFormat = [] {
            QTextCharFormat format;
            format.setBackground(QColor::fromRgb(180, 255, 145));
            return format;
        }();
        static QTextCharFormat const s_defaultInfoFormat = [] {
            QTextCharFormat format;
            format.setBackground(QColor::fromRgb(180, 145, 255));
            return format;
        }();
        static QTextCharFormat const s_defaultSpellingFormat = [] {
            QTextCharFormat format;
            format.setUnderlineStyle(QTextCharFormat::UnderlineStyle::WaveUnderline);
            format.setUnderlineColor(QColor::fromRgb(255, 0, 0));
            return format;
        }();
        static QTextCharFormat const s_defaultGrammarFormat = [] {
            QTextCharFormat format;
            format.setUnderlineStyle(QTextCharFormat::UnderlineStyle::WaveUnderline);
            format.setUnderlineColor(QColor::fromRgb(0, 0, 255));
            return format;
        }();

        switch (type) {
            case InsightType::Note:
                return s_defaultNoteFormat;
            case InsightType::Info:
                return s_defaultInfoFormat;
            case InsightType::SpellingError:
                return s_defaultSpellingFormat;
            case InsightType::GrammarError:
                return s_defaultGrammarFormat;
        }
        throw; // Should only ever happen if the enum is extended and this method isn't updated.
    }

    /**
     * Interface for all insights (notes, spell checking...)
     */
    class IInsight {
    public:
        virtual ~IInsight() = default;

        /**
         * @return Paragraphs spanned by this insight
         */
        virtual std::pair<int, int> parRange() const noexcept = 0;

        /**
         * Range of the insight in the document
         * @return Document position of first and last character included
         */
        virtual std::pair<int, int> range() const noexcept = 0;

        /**
         * @return A text cursor that spans the marked text
         */
        virtual QTextCursor toCursor() const noexcept = 0;

        /**
         * @return Insight message
         */
        virtual QString const& message() const noexcept = 0;

        /**
         * @return Type of this insight
         */
        virtual InsightType type() const noexcept = 0;

        /**
         * Provides a menu to show when the insight is right-clicked
         * @return The menu
         */
        virtual QMenu const& menu() const noexcept = 0;

    signals:
        /**
         * The insight's range has collapsed to zero length. It should probably be removed.
         */
        virtual void collapsed(IInsight*) = 0;
    };
}

Q_DECLARE_INTERFACE(novelist::IInsight, "novelist.IInsight")
Q_DECLARE_METATYPE(novelist::IInsight*)

#endif //NOVELIST_IINSPECTION_H
