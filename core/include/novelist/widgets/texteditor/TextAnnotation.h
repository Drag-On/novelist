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

    class TextAnnotation : public QObject, public IInsight {
    Q_OBJECT
    Q_INTERFACES(novelist::IInsight)

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
                InsightType type = InsightType::Note);

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
         * @return Type of this insight
         */
        InsightType type() const noexcept override;

        /**
         * @return Distance between left and right marker
         */
        int length() const noexcept;

        /**
         * @return A text cursor that spans the marked text
         */
        QTextCursor toCursor() const noexcept override;

        /**
         * Formatted output of annotation to a stream
         * @param s Stream
         * @param m Annotation
         * @return Stream
         */
        friend std::ostream& operator<<(std::ostream& s, TextAnnotation const& m) noexcept;

    signals:
        void collapsed(IInsight* insight) override;

    private:
        TextMarker m_marker;
        QString m_msg;
        InsightType m_type;
    };
}

#endif //NOVELIST_TEXTANNOTATION_H
