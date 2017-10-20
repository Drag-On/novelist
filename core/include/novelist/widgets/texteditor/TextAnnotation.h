/**********************************************************
 * @file   TextAnnotation.h
 * @author jan
 * @date   10/9/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TEXTANNOTATION_H
#define NOVELIST_TEXTANNOTATION_H

#include "document/TextMarker.h"
#include "TextEditor.h"

namespace novelist {
    /**
     * Available annotation types
     */
    enum class TextAnnotationType {
        Note, //!< Note style, will be displayed with a bright green background
        Info, //!< Info style, will be displayed with a bright violet background
        SpellingError, //!< Spelling error style, will be displayed with red squiggly underline
        GrammarError, //!< Grammar error style, will be displayed with blue squiggly underline
    };

    class TextAnnotation : public QObject, public IInsight {
    Q_OBJECT

    public:
        /**
         * Construct annotation for a range inside a document
         * @param doc Document
         * @param left Leftmost character
         * @param right Rightmost character
         * @param msg Message to show alongside the annotation
         * @param type Annotation type. This defines display style.
         */
        TextAnnotation(gsl::not_null<QTextDocument*> doc, int left, int right, QString msg,
                TextAnnotationType type = TextAnnotationType::Note);

        /**
         * @return Paragraphs spanned by this insight
         */
        std::pair<int, int> parRange() const noexcept override;

        /**
         * @return The marked range on the document
         */
        std::pair<int, int> range() const noexcept override;

        /**
         * Modify the marked range
         * @param left Left position
         * @param right Right position
         * @throw std::out_of_range if the provided range is invalid
         */
        void setRange(int left, int right);

        /**
         * @return The message
         */
        QString const& message() const noexcept override;

        /**
         * @param msg New message
         */
        void setMessage(QString msg) noexcept;

        /**
         * @return Human-readable type of this insight
         */
        QString type() const noexcept override;

        /**
         * @return Distance between left and right marker
         */
        int length() const noexcept;

        /**
         * @return A text cursor that spans the marked text
         */
        QTextCursor toCursor() const noexcept;

        /**
         * Formatted output of annotation to a stream
         * @param s Stream
         * @param m Annotation
         * @return Stream
         */
        friend std::ostream& operator<<(std::ostream& s, TextAnnotation const& m) noexcept;

    private:
        static QTextCharFormat const& typeToFormat(TextAnnotationType type);

        TextMarker m_marker;
        QString m_msg;
        TextAnnotationType m_type;

        inline static QTextCharFormat const s_defaultNoteFormat = [] {
            QTextCharFormat format;
            format.setBackground(QColor::fromRgb(180, 255, 145));
            return format;
        }();
        inline static QTextCharFormat const s_defaultInfoFormat = [] {
            QTextCharFormat format;
            format.setBackground(QColor::fromRgb(180, 145, 255));
            return format;
        }();
        inline static QTextCharFormat const s_defaultSpellingFormat = [] {
            QTextCharFormat format;
            format.setUnderlineStyle(QTextCharFormat::UnderlineStyle::WaveUnderline);
            format.setUnderlineColor(QColor::fromRgb(255, 0, 0));
            return format;
        }();
        inline static QTextCharFormat const s_defaultGrammarFormat = [] {
            QTextCharFormat format;
            format.setUnderlineStyle(QTextCharFormat::UnderlineStyle::WaveUnderline);
            format.setUnderlineColor(QColor::fromRgb(0, 0, 255));
            return format;
        }();
    };
}

#endif //NOVELIST_TEXTANNOTATION_H